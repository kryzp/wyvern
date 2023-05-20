
#include <backend/graphics/vulkan/vk_backend.h>
#include <backend/graphics/vulkan/vk_util.h>

#include <wvn/root.h>
#include <wvn/system/system_backend.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/graphics/render_pass.h>
#include <wvn/graphics/vertex.h>
#include <wvn/container/array.h>
#include <wvn/maths/vec3.h>

#include <backend/graphics/vulkan/vk_texture.h>

/*
 * =====================
 * VULKAN TODO LIST:
 * ---------------------
 * [ ] INSTANCED RENDERING
 * [ ] COMPUTE SHADERS
 * [ ] MULTIPLE SUBPASSES
 * [ ] MULTITHREADED COMMAND BUFFER GENERATION
 * [ ] SEPARATE IMAGES & SAMPLER DESCRIPTORS
 * [ ] PUSH CONSTANTS
 * =====================
 */

using namespace wvn;
using namespace wvn::gfx;

static const char* VALIDATION_LAYERS[] = {
	"VK_LAYER_KHRONOS_validation" // idk what the #define macro name for this is
};

static const char* DEVICE_EXTENSIONS[] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	"VK_KHR_portability_subset" // same here lol
};

static VkDynamicState DYNAMIC_STATES[] = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR,
	VK_DYNAMIC_STATE_BLEND_CONSTANTS
};

#if WVN_DEBUG

static bool g_debug_enable_validation_layers = false;

static bool debug_has_validation_layer_support()
{
	u32 layer_count = 0;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	VkLayerProperties available_layers[layer_count];
	vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

	for (int i = 0; i < ARRAY_LENGTH(VALIDATION_LAYERS); i++)
	{
		bool has_layer = false;
		const char* layer_name_0 = VALIDATION_LAYERS[i];

		for (int j = 0; j < layer_count; j++)
		{
			const char* layer_name_1 = available_layers[j].layerName;

			if (cstr::compare(layer_name_0, layer_name_1) == 0) {
				has_layer = true;
				break;
			}
		}

		if (!has_layer) {
			return false;
		}
	}

	return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
)
{
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		dev::LogMgr::get_singleton()->print("[VULKAN|DEBUG] Validation Layer (SEVERITY: %d) (TYPE: %d): %s", messageSeverity, messageType, pCallbackData->pMessage);
	}

	return VK_FALSE;
}

static VkResult debug_create_debug_utils_messenger_ext(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* create_info,
	const VkAllocationCallbacks* allocator,
	VkDebugUtilsMessengerEXT* debug_messenger
)
{
	auto fn = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (fn) {
		return fn(instance, create_info, allocator, debug_messenger);
	}

	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void debug_destroy_debug_utils_messenger_ext(
	VkInstance instance,
	VkDebugUtilsMessengerEXT messenger,
	const VkAllocationCallbacks* allocator
)
{
	auto fn = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (fn) {
		fn(instance, messenger, allocator);
	}
}

static void debug_populate_debug_utils_messenger_create_info_ext(VkDebugUtilsMessengerCreateInfoEXT* info)
{
	info->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	info->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	info->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	info->pfnUserCallback = vk_debug_callback;
	info->pUserData = nullptr;
}

#endif // WVN_DEBUG

static Vector<const char*> get_instance_extensions()
{
	u32 ext_count = 0;

	if (!Root::get_singleton()->system_backend()->vk_get_instance_extensions(&ext_count, nullptr)) {
		WVN_ERROR("[VULKAN|DEBUG] Unable to get instance extension count.");
	}

	Vector<const char*> extensions(ext_count);

	if (!Root::get_singleton()->system_backend()->vk_get_instance_extensions(&ext_count, extensions.data())) {
		WVN_ERROR("[VULKAN|DEBUG] Unable to get additional instance extensions.");
	}

#if WVN_DEBUG
	extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

	return extensions;
}

static Array<VkVertexInputAttributeDescription, 3> get_vertex_attribute_description()
{
	Array<VkVertexInputAttributeDescription, 3> result = {};

	// position part
	result[0].binding = 0;
	result[0].location = 0;
	result[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	result[0].offset = offsetof(Vertex, pos);

	// uv part
	result[1].binding = 0;
	result[1].location = 1;
	result[1].format = VK_FORMAT_R32G32_SFLOAT;
	result[1].offset = offsetof(Vertex, uv);

	// colour part
	result[2].binding = 0;
	result[2].location = 2;
	result[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	result[2].offset = offsetof(Vertex, col);

	return result;
}

static VkVertexInputBindingDescription get_vertex_binding_description()
{
	VkVertexInputBindingDescription result = {};
	result.binding = 0;
	result.stride = sizeof(Vertex);
	result.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return result;
}

VulkanBackend::VulkanBackend()
	: m_instance(VK_NULL_HANDLE)
	, m_surface(VK_NULL_HANDLE)
	, m_current_frame(0)
	, m_is_framebuffer_resized(false)
	, m_render_pass(VK_NULL_HANDLE)
	, m_render_pass_builder()
	, m_pipeline_layout(VK_NULL_HANDLE)
	, m_descriptor_set_layout(VK_NULL_HANDLE)
	, m_descriptor_pool_mgr(this)
	, m_descriptor_writes()
	, m_image_infos()
	, m_shader_stages()
	, m_pipeline_cache()
	, m_descriptor_set_cache()
	, m_pipeline_process_cache()
	, m_swap_chain(VK_NULL_HANDLE)
	, m_swap_chain_images()
	, m_swap_chain_image_views()
	, m_swap_chain_image_format()
	, m_swap_chain_extent()
	, m_swap_chain_framebuffers()
	, m_buffer_mgr(nullptr)
	, m_curr_image_idx(0)
	, m_texture_mgr(nullptr)
	, m_shader_mgr(nullptr)
	, frames()
	, queues()
	, device(VK_NULL_HANDLE)
	, physical_data()
	, m_depth(this)
	, m_depth_stencil_create_info()
	, m_colour(this)
	, m_msaa_samples(VK_SAMPLE_COUNT_1_BIT)
#if WVN_DEBUG
	, m_debug_messenger()
#endif
{
	VkApplicationInfo app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = Root::get_singleton()->config().name,
		.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
		.pEngineName = "Wyvern",
		.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
		.apiVersion = VK_API_VERSION_1_3
	};

	VkInstanceCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app_info
	};

#if WVN_DEBUG
	VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};

	if (debug_has_validation_layer_support())
	{
		dev::LogMgr::get_singleton()->print("[VULKAN|DEBUG] Validation layer support verified!");

		debug_populate_debug_utils_messenger_create_info_ext(&debug_create_info);

		create_info.enabledLayerCount = ARRAY_LENGTH(VALIDATION_LAYERS);
		create_info.ppEnabledLayerNames = VALIDATION_LAYERS;
		create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;

		g_debug_enable_validation_layers = true;
	}
	else
	{
		dev::LogMgr::get_singleton()->print("[VULKAN|DEBUG] No validation layer support.");

		create_info.enabledLayerCount = 0;
		create_info.ppEnabledLayerNames = nullptr;
		create_info.pNext = nullptr;

		g_debug_enable_validation_layers = false;
	}
#endif

	auto extensions = get_instance_extensions();
	create_info.enabledExtensionCount = extensions.size();
	create_info.ppEnabledExtensionNames = extensions.data();

	create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

	if (VkResult result = vkCreateInstance(&create_info, nullptr, &m_instance); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to create instance: %d", result);
	}

#if WVN_DEBUG
	if (VkResult result = debug_create_debug_utils_messenger_ext(m_instance, &debug_create_info, nullptr, &m_debug_messenger); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to create debug messenger: %d", result);
	}
#endif

	// create surface
	if (bool result = Root::get_singleton()->system_backend()->vk_create_surface(m_instance, &m_surface); !result) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to create surface: %d", result);
	}

	enumerate_physical_devices();

	QueueFamilyIdx phys_idx = find_queue_families(physical_data.device);

	m_depth_stencil_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	m_depth_stencil_create_info.depthTestEnable  = VK_TRUE;
	m_depth_stencil_create_info.depthWriteEnable = VK_TRUE;
	m_depth_stencil_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
	m_depth_stencil_create_info.depthBoundsTestEnable = VK_FALSE;
	m_depth_stencil_create_info.minDepthBounds = 0.0f;
	m_depth_stencil_create_info.maxDepthBounds = 1.0f;
	m_depth_stencil_create_info.stencilTestEnable = VK_FALSE;
	m_depth_stencil_create_info.front = {};
	m_depth_stencil_create_info.back  = {};

	create_logical_device(phys_idx);
	create_swap_chain(phys_idx);
	create_swap_chain_image_views();
	create_render_pass();
	create_descriptor_set_layout();
	create_pipeline_layout();
	create_pipeline_process_cache();
	create_command_pools(phys_idx);
	create_colour_resources();
	create_depth_resources();
	create_swap_chain_framebuffers();
	create_uniform_buffers();
	m_descriptor_pool_mgr.init();
	create_command_buffers();
	create_sync_objects();

	m_buffer_mgr  = new VulkanBufferMgr(this);
	m_texture_mgr = new VulkanTextureMgr(this);
	m_shader_mgr  = new VulkanShaderMgr(this);

	acquire_next_image();

	// finished :D
	dev::LogMgr::get_singleton()->print("[VULKAN] Initialized!");
}

VulkanBackend::~VulkanBackend()
{
	vkDeviceWaitIdle(this->device); // sync up

	// //

	clean_up_swap_chain();

	delete m_texture_mgr;
	delete m_buffer_mgr;
	delete m_shader_mgr;

	m_depth.clean_up();
	m_colour.clean_up();

	clear_pipeline_cache();

	vkDestroyPipelineCache(this->device, m_pipeline_process_cache, nullptr);
	vkDestroyPipelineLayout(this->device, m_pipeline_layout, nullptr);
	vkDestroyRenderPass(this->device, m_render_pass, nullptr);

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		delete frames[i].uniform_buffer;
	}

	clear_descriptor_set_cache();

	m_descriptor_pool_mgr.clean_up();
	vkDestroyDescriptorSetLayout(this->device, m_descriptor_set_layout, nullptr);

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyFence(this->device, frames[i].in_flight_fence, nullptr);
		vkDestroySemaphore(this->device, frames[i].render_finished_semaphore, nullptr);
		vkDestroySemaphore(this->device, frames[i].image_available_semaphore, nullptr);
	}

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyCommandPool(this->device, frames[i].command_pool, nullptr);
	}

	vkDestroyDevice(this->device, nullptr);

#if WVN_DEBUG
	if (g_debug_enable_validation_layers) {
		debug_destroy_debug_utils_messenger_ext(m_instance, m_debug_messenger, nullptr);
		dev::LogMgr::get_singleton()->print("[VULKAN|DEBUG] Destroyed validation layers!");
	}
#endif

	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkDestroyInstance(m_instance, nullptr);

	// //

	dev::LogMgr::get_singleton()->print("[VULKAN] Destroyed!");
}

RendererBackendProperties VulkanBackend::properties() { return {
	.origin_bottom_left = false
}; }

void VulkanBackend::enumerate_physical_devices()
{
	u32 device_count = 0;
	vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);

	if (!device_count) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to find GPUs with Vulkan support!");
	}

	VkPhysicalDeviceProperties properties = {};
	VkPhysicalDeviceFeatures features = {};

	Vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

	vkGetPhysicalDeviceProperties(devices[0], &properties);
	vkGetPhysicalDeviceFeatures(devices[0], &features);

	physical_data.device = devices[0];
	physical_data.properties = properties;
	physical_data.features = features;

	m_msaa_samples = get_max_usable_sample_count();

	bool has_essentials = false;
	u32 usability0 = assign_physical_device_usability(physical_data.device, properties, features, &has_essentials);

	// select the device of the highest usability
	for (int i = 1; i < device_count; i++)
	{
		vkGetPhysicalDeviceProperties(devices[i], &physical_data.properties);
		vkGetPhysicalDeviceFeatures(devices[i], &physical_data.features);

		u32 usability1 = assign_physical_device_usability(devices[i], properties, features, &has_essentials);

		if (usability1 > usability0 && has_essentials)
		{
			usability0 = usability1;

			physical_data.device = devices[i];
			physical_data.properties = properties;
			physical_data.features = features;

			m_msaa_samples = get_max_usable_sample_count();
		}
	}

	if (physical_data.device == VK_NULL_HANDLE) {
		WVN_ERROR("[VULKAN|DEBUG] Unable to find a suitable GPU!");
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Selected a suitable GPU: %d", physical_data.device);
}

void VulkanBackend::create_logical_device(const QueueFamilyIdx& phys_idx)
{
	constexpr float QUEUE_PRIORITY = 1.0f;

	Vector<VkDeviceQueueCreateInfo> queue_create_infos;

	for (auto& family : phys_idx.package())
	{
		queue_create_infos.push_back({
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = family,
			.queueCount = 1,
			.pQueuePriorities = &QUEUE_PRIORITY
		});
	}

	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.queueCreateInfoCount = static_cast<u32>(queue_create_infos.size());
	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.enabledLayerCount = 0;
	create_info.ppEnabledLayerNames = nullptr;
	create_info.ppEnabledExtensionNames = DEVICE_EXTENSIONS;
	create_info.enabledExtensionCount = static_cast<u32>(ARRAY_LENGTH(DEVICE_EXTENSIONS));
	create_info.pEnabledFeatures = &physical_data.features;

#if WVN_DEBUG
	if (g_debug_enable_validation_layers) {
		create_info.enabledLayerCount = ARRAY_LENGTH(VALIDATION_LAYERS);
		create_info.ppEnabledLayerNames = VALIDATION_LAYERS;
	}
	dev::LogMgr::get_singleton()->print("[VULKAN|DEBUG] Enabled validation layers!");
#endif

	if (VkResult result = vkCreateDevice(physical_data.device, &create_info, nullptr, &this->device); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to create logical device: %d", result);
	}

	vkGetDeviceQueue(this->device, phys_idx.graphics_family.value(), 0, &queues.graphics);
	vkGetDeviceQueue(this->device, phys_idx.present_family.value(),  0, &queues.present);
	vkGetDeviceQueue(this->device, phys_idx.compute_family.value(),  0, &queues.compute);
	vkGetDeviceQueue(this->device, phys_idx.transfer_family.value(), 0, &queues.transfer);

	dev::LogMgr::get_singleton()->print("[VULKAN] Created a logical device!");
}

void VulkanBackend::create_swap_chain(const QueueFamilyIdx& phys_idx)
{
	SwapChainSupportDetails details = query_swap_chain_support(physical_data.device);

	auto surf_fmt  = vkutil::choose_swap_surface_format(details.surface_formats);
	auto pres_mode = vkutil::choose_swap_present_mode(details.present_modes);
	auto extent    = vkutil::choose_swap_extent(details.capabilities);

	u32 img_count = details.capabilities.minImageCount + 1;

	if (details.capabilities.maxImageCount > 0 && img_count > details.capabilities.maxImageCount) {
		img_count = details.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = m_surface;
	create_info.minImageCount = img_count;
	create_info.imageFormat = surf_fmt.format;
	create_info.imageColorSpace = surf_fmt.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	u32 idxs[2] = { phys_idx.graphics_family.value(), phys_idx.present_family.value() };

	if (phys_idx.all_unique())
	{
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = idxs;
	}
	else
	{
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = nullptr;
	}

	create_info.preTransform = details.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = pres_mode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	if (VkResult result = vkCreateSwapchainKHR(this->device, &create_info, nullptr, &m_swap_chain); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to create swap chain: %d", result);
	}

	vkGetSwapchainImagesKHR(this->device, m_swap_chain, &img_count, nullptr);

	if (!img_count) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to find any images in swap chain!");
	}

	m_swap_chain_images.resize(img_count);

	VkImage images[img_count];
	vkGetSwapchainImagesKHR(this->device, m_swap_chain, &img_count, images);

	for (int i = 0; i < img_count; i++) {
		m_swap_chain_images[i] = images[i];
	}

	this->m_swap_chain_image_format = surf_fmt.format;
	this->m_swap_chain_extent = extent;

	dev::LogMgr::get_singleton()->print("[VULKAN] Created the swap chain!");
}

void VulkanBackend::create_swap_chain_image_views()
{
	m_swap_chain_image_views.resize(m_swap_chain_images.size());

	for (u64 i = 0; i < m_swap_chain_images.size(); i++)
	{
		VkImageViewCreateInfo view_info = {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = m_swap_chain_images[i];
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = m_swap_chain_image_format;

		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		if (VkResult result = vkCreateImageView(this->device, &view_info, nullptr, &m_swap_chain_image_views[i]); result != VK_SUCCESS) {
			WVN_ERROR("[VULKAN|DEBUG] Failed to create texture image view: %d", result);
		}
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created swap chain image views!");
}

void VulkanBackend::create_pipeline_process_cache()
{
	VkPipelineCacheCreateInfo pipeline_cache_create_info = {};
	pipeline_cache_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	pipeline_cache_create_info.pNext = nullptr;
	pipeline_cache_create_info.flags = 0;
	pipeline_cache_create_info.initialDataSize = 0;
	pipeline_cache_create_info.pInitialData = nullptr;

	if (VkResult result = vkCreatePipelineCache(device, &pipeline_cache_create_info, nullptr, &m_pipeline_process_cache); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to process pipeline cache: %d", result);
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created graphics pipeline process cache!");
}

VkPipeline VulkanBackend::get_graphics_pipeline()
{
	auto binding_desc = get_vertex_binding_description();
	auto attribs_desc = get_vertex_attribute_description();

	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {};
	vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
	vertex_input_state_create_info.pVertexBindingDescriptions = &binding_desc;
	vertex_input_state_create_info.vertexAttributeDescriptionCount = attribs_desc.size();
	vertex_input_state_create_info.pVertexAttributeDescriptions = attribs_desc.data();

	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {};
	input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_swap_chain_extent.width);
	viewport.height = static_cast<float>(m_swap_chain_extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = m_swap_chain_extent;

	VkPipelineViewportStateCreateInfo viewport_state_create_info = {};
	viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state_create_info.viewportCount = 1;
	viewport_state_create_info.pViewports = &viewport;
	viewport_state_create_info.scissorCount = 1;
	viewport_state_create_info.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {};
	rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state_create_info.depthClampEnable = VK_FALSE;
	rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
	rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization_state_create_info.lineWidth = 1.0f;
	rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterization_state_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterization_state_create_info.depthBiasEnable = VK_FALSE;
	rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
	rasterization_state_create_info.depthBiasClamp = 0.0f;
	rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {};
	multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_state_create_info.sampleShadingEnable = VK_TRUE; // todo this should be toggleable! especially for things like pixel art-styles (originally VK_FALSE)
	multisample_state_create_info.minSampleShading = 0.2f; // todo this should be variable (originally 1.0f)
	multisample_state_create_info.rasterizationSamples = m_msaa_samples;
	multisample_state_create_info.pSampleMask = nullptr;
	multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
	multisample_state_create_info.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colour_blend_attachment_state = {};
	colour_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colour_blend_attachment_state.blendEnable = VK_TRUE;
	colour_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colour_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colour_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
	colour_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colour_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colour_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colour_blend_state_create_info = {};
	colour_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colour_blend_state_create_info.logicOpEnable = VK_FALSE;
	colour_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
	colour_blend_state_create_info.attachmentCount = 1;
	colour_blend_state_create_info.pAttachments = &colour_blend_attachment_state;
	colour_blend_state_create_info.blendConstants[0] = 0.0f;
	colour_blend_state_create_info.blendConstants[1] = 0.0f;
	colour_blend_state_create_info.blendConstants[2] = 0.0f;
	colour_blend_state_create_info.blendConstants[3] = 0.0f;

	u32 created_pipeline_hash = 0;

	hash::combine(&created_pipeline_hash, colour_blend_attachment_state);
	hash::combine(&created_pipeline_hash, rasterization_state_create_info);
	hash::combine(&created_pipeline_hash, input_assembly_state_create_info);
	hash::combine(&created_pipeline_hash, multisample_state_create_info);
	hash::combine(&created_pipeline_hash, m_render_pass);

	auto xxx = get_vertex_attribute_description();
	auto yyy = get_vertex_binding_description();

	for (int i = 0; i < xxx.size(); i++) {
		hash::combine(&created_pipeline_hash, xxx[i]);
	}

	hash::combine(&created_pipeline_hash, yyy);

	for (int i = 0; i < 2; i++) { // TODO TODO: Should be "m_shader_stages.size()", but only using vertex + fragment right now so its hardcoded to 2
		hash::combine(&created_pipeline_hash, m_shader_stages[i]);
	}

	if (m_pipeline_cache.contains(created_pipeline_hash)) {
		return m_pipeline_cache[created_pipeline_hash];
	}

	VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {};
	dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_create_info.dynamicStateCount = static_cast<u32>(ARRAY_LENGTH(DYNAMIC_STATES));
	dynamic_state_create_info.pDynamicStates = DYNAMIC_STATES;

	VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {};
	graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphics_pipeline_create_info.pStages = m_shader_stages.data();
	graphics_pipeline_create_info.stageCount = 2; // TODO TODO: Should be "m_shader_stages.size()", but only using vertex + fragment right now so its hardcoded to 2
	graphics_pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
	graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
	graphics_pipeline_create_info.pViewportState = &viewport_state_create_info;
	graphics_pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
	graphics_pipeline_create_info.pMultisampleState = &multisample_state_create_info;
	graphics_pipeline_create_info.pDepthStencilState = &m_depth_stencil_create_info;
	graphics_pipeline_create_info.pColorBlendState = &colour_blend_state_create_info;
	graphics_pipeline_create_info.pDynamicState = &dynamic_state_create_info;
	graphics_pipeline_create_info.layout = m_pipeline_layout;
	graphics_pipeline_create_info.renderPass = m_render_pass;
	graphics_pipeline_create_info.subpass = 0;
	graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
	graphics_pipeline_create_info.basePipelineIndex = -1;

	VkPipeline created_pipeline = VK_NULL_HANDLE;

	if (VkResult result = vkCreateGraphicsPipelines(this->device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &created_pipeline); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to create new graphics pipeline: %d", result);
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created new graphics pipeline!");

	m_pipeline_cache.insert(Pair(created_pipeline_hash, created_pipeline));

	return created_pipeline;
}

void VulkanBackend::create_render_pass()
{
	Array<VkAttachmentDescription, 3> attachment_descriptions;

	// colour attachment
	attachment_descriptions[0].format = m_swap_chain_image_format;
	attachment_descriptions[0].samples = m_msaa_samples;
	attachment_descriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_descriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_descriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // todo: we dont use the stencil buffer YET
	attachment_descriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_descriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_descriptions[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference swap_chain_colour_attachment_ref = {};
	swap_chain_colour_attachment_ref.attachment = 0;
	swap_chain_colour_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// colour attachment resolve
	attachment_descriptions[2].format = m_swap_chain_image_format;
	attachment_descriptions[2].samples = VK_SAMPLE_COUNT_1_BIT;
	attachment_descriptions[2].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_descriptions[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_descriptions[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_descriptions[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_descriptions[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_descriptions[2].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colour_attachment_resolve_ref = {};
	colour_attachment_resolve_ref.attachment = 2;
	colour_attachment_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// depth attachment
	attachment_descriptions[1].format = vkutil::find_depth_format(physical_data.device);
	attachment_descriptions[1].samples = m_msaa_samples;
	attachment_descriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_descriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_descriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_descriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_descriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_descriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attachment_ref = {};
	depth_attachment_ref.attachment = 1;
	depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// //

	VkSubpassDescription swap_chain_sub_pass = {};
	swap_chain_sub_pass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	swap_chain_sub_pass.pColorAttachments = &swap_chain_colour_attachment_ref;
	swap_chain_sub_pass.pResolveAttachments = &colour_attachment_resolve_ref;
	swap_chain_sub_pass.colorAttachmentCount = 1;
	swap_chain_sub_pass.pDepthStencilAttachment = &depth_attachment_ref;

	VkSubpassDependency sub_pass_dependency = {};
	sub_pass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	sub_pass_dependency.dstSubpass = 0;
	sub_pass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	sub_pass_dependency.srcAccessMask = 0;
	sub_pass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	sub_pass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo render_pass_create_info = {};
	render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_create_info.pAttachments = attachment_descriptions.data();
	render_pass_create_info.attachmentCount = attachment_descriptions.size();
	render_pass_create_info.pSubpasses = &swap_chain_sub_pass;
	render_pass_create_info.subpassCount = 1;
	render_pass_create_info.pDependencies = &sub_pass_dependency;
	render_pass_create_info.dependencyCount = 1;

	if (VkResult result = vkCreateRenderPass(this->device, &render_pass_create_info, nullptr, &m_render_pass); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to create render pass: %d", result);
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created render pass!");
}

void VulkanBackend::create_swap_chain_framebuffers()
{
	m_swap_chain_framebuffers.resize(m_swap_chain_image_views.size());

	for (u64 i = 0; i < m_swap_chain_image_views.size(); i++)
	{
		Array<VkImageView, 3> attachments;

		attachments[0] = m_colour.image_view();
		attachments[1] = m_depth.image_view();
		attachments[2] = m_swap_chain_image_views[i];

		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = m_render_pass;
		framebuffer_info.pAttachments = attachments.data();
		framebuffer_info.attachmentCount = attachments.size();
		framebuffer_info.width = m_swap_chain_extent.width;
		framebuffer_info.height = m_swap_chain_extent.height;
		framebuffer_info.layers = 1;

		if (VkResult result = vkCreateFramebuffer(this->device, &framebuffer_info, nullptr, &m_swap_chain_framebuffers[i]); result != VK_SUCCESS) {
			WVN_ERROR("[VULKAN|DEBUG] Failed to create framebuffer: (Index: %llu), %d", i, result);
		}
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created swap chain framebuffers!");
}

void VulkanBackend::clean_up_swap_chain()
{
	for (auto& fbo : m_swap_chain_framebuffers) {
		vkDestroyFramebuffer(this->device, fbo, nullptr);
	}

	for (auto& view : m_swap_chain_image_views) {
		vkDestroyImageView(this->device, view, nullptr);
	}

	vkDestroySwapchainKHR(this->device, m_swap_chain, nullptr);
}

void VulkanBackend::rebuild_swap_chain()
{
	// wait until draw size isn't zero
	while (Root::get_singleton()->system_backend()->get_draw_size() == Vec2I::zero()) { }
	vkDeviceWaitIdle(this->device);

	clean_up_swap_chain();

	create_swap_chain(find_queue_families(physical_data.device));
	create_swap_chain_image_views();
	create_colour_resources();
	create_depth_resources();
	create_swap_chain_framebuffers();
}

void VulkanBackend::create_command_pools(const QueueFamilyIdx& phys_idx)
{
	VkCommandPoolCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	create_info.queueFamilyIndex = phys_idx.graphics_family.value();

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (VkResult result = vkCreateCommandPool(this->device, &create_info, nullptr, &frames[i].command_pool); result != VK_SUCCESS) {
			WVN_ERROR("[VULKAN|DEBUG] Failed to create command pools: %d", result);
		}
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created command pool!");
}

void VulkanBackend::create_command_buffers()
{
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
		command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_allocate_info.commandPool = frames[i].command_pool;
		command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_allocate_info.commandBufferCount = 1;

		if (VkResult result = vkAllocateCommandBuffers(this->device, &command_buffer_allocate_info, &frames[i].command_buffer); result != VK_SUCCESS) {
			WVN_ERROR("[VULKAN|DEBUG] Failed to create command buffers: %d", result);
		}
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created command buffer!");
}

void VulkanBackend::create_sync_objects()
{
	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (VkResult result = vkCreateSemaphore(this->device, &semaphore_create_info, nullptr, &frames[i].image_available_semaphore); result != VK_SUCCESS) {
			WVN_ERROR("[VULKAN|DEBUG] Failed to create image available semaphore: %d", result);
		}

		if (VkResult result = vkCreateSemaphore(this->device, &semaphore_create_info, nullptr, &frames[i].render_finished_semaphore); result != VK_SUCCESS) {
			WVN_ERROR("[VULKAN|DEBUG] Failed to create render finished semaphore: %d", result);
		}

		if (VkResult result = vkCreateFence(this->device, &fence_create_info, nullptr, &frames[i].in_flight_fence); result != VK_SUCCESS) {
			WVN_ERROR("[VULKAN|DEBUG] Failed to create in flight fence: %d", result);
		}
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created sync objects!");
}

void VulkanBackend::create_descriptor_set_layout()
{
	int n_tex = 0;
	for (; n_tex < m_image_infos.size(); n_tex++) {
		if (!m_image_infos[n_tex].imageView) {
			break;
		}
	}

	Array<VkDescriptorSetLayoutBinding, 1 + WVN_MAX_BOUND_TEXTURES> bindings;

	// ubo binding
	bindings[0].binding = 0;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	bindings[0].descriptorCount = 1;
	bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	bindings[0].pImmutableSamplers = nullptr;

	// samplers
	for (int i = 0; i < WVN_MAX_BOUND_TEXTURES; i++) {
		int idx = i + 1;
		bindings[idx].binding = idx;
		bindings[idx].descriptorCount = 1;
		bindings[idx].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		bindings[idx].pImmutableSamplers = nullptr;
		bindings[idx].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	}

	VkDescriptorSetLayoutCreateInfo layout_create_info = {};
	layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_create_info.bindingCount = 1 + WVN_MAX_BOUND_TEXTURES;
	layout_create_info.pBindings = bindings.data();

	if (VkResult result = vkCreateDescriptorSetLayout(this->device, &layout_create_info, nullptr, &m_descriptor_set_layout); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to create descriptor set layout: %d", result);
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created descriptor set layout!");
}

void VulkanBackend::create_pipeline_layout()
{
	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.setLayoutCount = 1;
	pipeline_layout_create_info.pSetLayouts = &m_descriptor_set_layout;
	pipeline_layout_create_info.pushConstantRangeCount = 0;
	pipeline_layout_create_info.pPushConstantRanges = nullptr;

	if (VkResult result = vkCreatePipelineLayout(this->device, &pipeline_layout_create_info, nullptr, &m_pipeline_layout); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to create pipeline layout: %d", result);
	}
}

VkDescriptorSet VulkanBackend::get_descriptor_set()
{
	u32 descriptor_set_hash = 0;

	int n_tex = 0;
	for (; n_tex < m_image_infos.size(); n_tex++) {
		if (!m_image_infos[n_tex].imageView) {
			break;
		} else {
			hash::combine(&descriptor_set_hash, m_image_infos[n_tex]);
		}
	}

	hash::combine(&descriptor_set_hash, m_current_frame);

	if (m_descriptor_set_cache.contains(descriptor_set_hash)) {
		return m_descriptor_set_cache[descriptor_set_hash];
	}

	VkDescriptorSet descriptor_set_result = m_descriptor_pool_mgr.allocate_descriptor_set(m_descriptor_set_layout);

	VkDescriptorBufferInfo descriptor_buffer_info = {};
	descriptor_buffer_info.buffer = current_frame().uniform_buffer->buffer();
	//descriptor_buffer_info.offset = 0;//jjj * sizeof(UniformBufferObject);
	descriptor_buffer_info.range = sizeof(UniformBufferObject);

	m_descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	m_descriptor_writes[0].dstSet = descriptor_set_result;
	m_descriptor_writes[0].dstBinding = 0;
	m_descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	m_descriptor_writes[0].descriptorCount = 1;
	m_descriptor_writes[0].pBufferInfo = &descriptor_buffer_info;

	for (int i = 0; i < WVN_MAX_BOUND_TEXTURES; i++) {
		int idx = i + 1;
		m_descriptor_writes[idx].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		m_descriptor_writes[idx].dstSet = descriptor_set_result;
		m_descriptor_writes[idx].dstBinding = idx;
		m_descriptor_writes[idx].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		m_descriptor_writes[idx].descriptorCount = 1;
		m_descriptor_writes[idx].pImageInfo = &m_image_infos[i];
	}

	vkUpdateDescriptorSets(this->device, 1 + n_tex, m_descriptor_writes.data(), 0, nullptr);

	dev::LogMgr::get_singleton()->print("[VULKAN] Created new descriptor set!");

	m_descriptor_set_cache.insert(Pair(descriptor_set_hash, descriptor_set_result));

	return descriptor_set_result;
}

void VulkanBackend::create_uniform_buffers()
{
	VkDeviceSize buffer_size = sizeof(UniformBufferObject) * MAX_UBOS;

	for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		frames[i].uniform_buffer = new VulkanBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		frames[i].uniform_buffer->create(
			this,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			buffer_size
		);
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created uniform buffers!");
}

void VulkanBackend::create_depth_resources()
{
	VkFormat format = vkutil::find_depth_format(physical_data.device);

	m_depth.create(
		m_swap_chain_extent.width, m_swap_chain_extent.height,
		vkutil::get_wvn_texture_format(format), TEX_TILE_OPTIMAL, TEX_TYPE_2D,
		1, m_msaa_samples, false
	);

	m_depth.transition_layout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	dev::LogMgr::get_singleton()->print("[VULKAN] Created depth resources!");
}

void VulkanBackend::create_colour_resources()
{
	VkFormat format = m_swap_chain_image_format;

	m_colour.create(
		m_swap_chain_extent.width, m_swap_chain_extent.height,
		vkutil::get_wvn_texture_format(format), TEX_TILE_OPTIMAL, TEX_TYPE_2D,
		1, m_msaa_samples, true
	);

	dev::LogMgr::get_singleton()->print("[VULKAN] Created colour resources!");
}

u32 VulkanBackend::assign_physical_device_usability(
	VkPhysicalDevice physical_device,
	VkPhysicalDeviceProperties properties,
	VkPhysicalDeviceFeatures features,
	bool* essentials_completed
)
{
	u32 result = 0;

	bool adequate_swap_chain = false;
	bool has_required_extensions = check_device_extension_support(physical_device);
	bool indices_complete = find_queue_families(physical_device).is_complete();
	bool has_anisotropy = features.samplerAnisotropy;

	if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		result += 2;
	} else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
		result += 1;
	}

	if (features.geometryShader) {
		result += 1;
	}

	if (has_anisotropy) {
		result += 1;
	}

	if (has_required_extensions) {
		SwapChainSupportDetails swap_chain_support_details = query_swap_chain_support(physical_device);
		adequate_swap_chain = swap_chain_support_details.surface_formats.any() && swap_chain_support_details.present_modes.any();
		result += 1;
	}

	if (essentials_completed) {
		(*essentials_completed) = indices_complete && has_required_extensions && adequate_swap_chain && has_anisotropy;
	}

	return result;
}

QueueFamilyIdx VulkanBackend::find_queue_families(VkPhysicalDevice physical_device)
{
	QueueFamilyIdx result = {};

	u32 queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

	if (!queue_family_count) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to find any queue families!");
	}

	Vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

	for (int i = 0; i < queue_family_count; i++)
	{
		if ((queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !result.graphics_family.has_value()) {
			result.graphics_family = i;
			continue;
		}

		if ((queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && !result.compute_family.has_value()) {
			result.compute_family = i;
			continue;
		}

		if ((queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && !result.transfer_family.has_value()) {
			result.transfer_family = i;
			continue;
		}

		VkBool32 present_support = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, m_surface, &present_support);

		if (present_support && !result.present_family.has_value()) {
			result.present_family = i;
			continue;
		}

		if (result.is_complete()) {
			break;
		}
	}

	return result;
}

bool VulkanBackend::check_device_extension_support(VkPhysicalDevice physical_device)
{
	u32 ext_count = 0;
	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &ext_count, nullptr);

	if (!ext_count) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to find any device extension properties!");
	}

	Vector<VkExtensionProperties> available_exts(ext_count);
	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &ext_count, available_exts.data());

	Vector<const char*> required_exts(DEVICE_EXTENSIONS, ARRAY_LENGTH(DEVICE_EXTENSIONS));

	for (const auto& available_extension : available_exts) {
		for (int i = 0; i < required_exts.size(); i++) {
			if (cstr::compare(available_extension.extensionName, required_exts[i])) {
				required_exts.erase(i);
			}
		}
	}

	return required_exts.empty();
}

SwapChainSupportDetails VulkanBackend::query_swap_chain_support(VkPhysicalDevice physical_device)
{
	SwapChainSupportDetails result = {};

	// get capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, m_surface, &result.capabilities);

	// get surface formats
	u32 surf_fmt_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, m_surface, &surf_fmt_count, nullptr);

	if (surf_fmt_count)
	{
		result.surface_formats.resize(surf_fmt_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, m_surface, &surf_fmt_count, result.surface_formats.data());
	}

	// get present modes
	u32 present_mode_count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, m_surface, &present_mode_count, nullptr);

	if (present_mode_count)
	{
		result.present_modes.resize(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, m_surface, &present_mode_count, result.present_modes.data());
	}

	return result;
}

void VulkanBackend::clear_pipeline_cache()
{
	for (auto& [id, cache] : m_pipeline_cache) {
		vkDestroyPipeline(this->device, cache, 0);
	}

	m_pipeline_cache.clear();
}

void VulkanBackend::clear_descriptor_set_cache()
{
	m_descriptor_set_cache.clear();
}

VkSampleCountFlagBits VulkanBackend::get_max_usable_sample_count() const
{
	VkSampleCountFlags counts =
		physical_data.properties.limits.framebufferColorSampleCounts &
		physical_data.properties.limits.framebufferDepthSampleCounts;

	if (counts & VK_SAMPLE_COUNT_64_BIT) {
		return VK_SAMPLE_COUNT_64_BIT;
	} else if (counts & VK_SAMPLE_COUNT_32_BIT) {
		return VK_SAMPLE_COUNT_32_BIT;
	} else if (counts & VK_SAMPLE_COUNT_16_BIT) {
		return VK_SAMPLE_COUNT_16_BIT;
	} else if (counts & VK_SAMPLE_COUNT_8_BIT) {
		return VK_SAMPLE_COUNT_8_BIT;
	} else if (counts & VK_SAMPLE_COUNT_4_BIT) {
		return VK_SAMPLE_COUNT_4_BIT;
	} else if (counts & VK_SAMPLE_COUNT_2_BIT) {
		return VK_SAMPLE_COUNT_2_BIT;
	} else {
		return VK_SAMPLE_COUNT_1_BIT;
	}
}

void VulkanBackend::on_window_resize(int width, int height)
{
	m_is_framebuffer_resized = true;
}

void VulkanBackend::set_texture(u32 idx, const Texture* texture)
{
	if (!texture) {
		m_image_infos[idx].imageView = VK_NULL_HANDLE;
		return;
	}

	m_image_infos[idx].imageView = static_cast<const VulkanTexture*>(texture)->image_view();
	m_image_infos[idx].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void VulkanBackend::set_sampler(u32 idx, TextureSampler* sampler)
{
	m_image_infos[idx].sampler = static_cast<VulkanTextureSampler*>(sampler)->bind(device, physical_data.properties, 4);
}

void VulkanBackend::bind_shader(Shader* shader)
{
	VulkanShader* vksh = static_cast<VulkanShader*>(shader);
	m_shader_stages[vksh->type] = vksh->get_shader_stage_create_info();
}

VulkanBackend::FrameData& VulkanBackend::current_frame()
{
	return frames[m_current_frame];
}

u64 VulkanBackend::frame() const
{
	return m_current_frame;
}

void VulkanBackend::wait_for_sync()
{
	vkWaitForFences(this->device, 1, &current_frame().in_flight_fence, VK_TRUE, UINT64_MAX);
}

void VulkanBackend::begin_render()
{
	const float ASPECT = static_cast<float>(m_swap_chain_extent.width) / static_cast<float>(m_swap_chain_extent.height);

	vkResetCommandPool(this->device, current_frame().command_pool, 0);
	VkCommandBuffer current_buffer = current_frame().command_buffer;

	VkCommandBufferBeginInfo command_buffer_begin_info = {};
	command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (VkResult result = vkBeginCommandBuffer(current_buffer, &command_buffer_begin_info); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to begin recording command buffer: %d", result);
	}

	VkRenderPassBeginInfo render_pass_begin_info = m_render_pass_builder.begin_info(
		m_render_pass,
		m_swap_chain_framebuffers[m_curr_image_idx],
		m_swap_chain_extent
	);

	vkCmdBeginRenderPass(current_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanBackend::render(const RenderPass& pass)
{
	VkCommandBuffer current_buffer = current_frame().command_buffer;

	UniformBufferObject ubo = {};

	ubo.model = Mat4x4(
		pass.model_matrix.m11, pass.model_matrix.m12, pass.model_matrix.m13, 0.0f,
		pass.model_matrix.m21, pass.model_matrix.m22, pass.model_matrix.m23, 0.0f,
		pass.model_matrix.m31, pass.model_matrix.m32, pass.model_matrix.m33, 0.0f,
		pass.model_matrix.m14, pass.model_matrix.m24, pass.model_matrix.m34, 1.0f
	);

	ubo.view = pass.view_matrix;
	ubo.proj = pass.proj_matrix;
	ubo.proj.m22 *= -1.0f; // -y is up, simplest way to do this it seems. that and making sure we're using COUNTER_CLOCKWISE rendering.

	u32 ubo_dynamic_offset = sizeof(UniformBufferObject) * jjj;
	current_frame().uniform_buffer->read_data(&ubo, sizeof(UniformBufferObject), sizeof(UniformBufferObject) * jjj);

	VkViewport viewport = {};

	if (pass.viewport.has_value())
	{
		const auto& pass_viewport = pass.viewport.value();

		viewport.x = pass_viewport.x;
		viewport.y = pass_viewport.y;
		viewport.width = pass_viewport.w;
		viewport.height = pass_viewport.h;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
	}
	else
	{
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)m_swap_chain_extent.width;
		viewport.height = (float)m_swap_chain_extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
	}

	VkRect2D scissor = {};

	if (pass.scissor.has_value())
	{
		const auto& pass_scissor = pass.scissor.value();

		scissor.offset.x = pass_scissor.x;
		scissor.offset.y = pass_scissor.y;
		scissor.extent.width = pass_scissor.w;
		scissor.extent.height = pass_scissor.h;
	}
	else
	{
		scissor.offset = { 0, 0 };
		scissor.extent = m_swap_chain_extent;
	}

	const auto& vertex_buffer = static_cast<VulkanBuffer*>(pass.mesh->vertex_buffer())->buffer();
	const auto& index_buffer = static_cast<VulkanBuffer*>(pass.mesh->index_buffer())->buffer();

	vkCmdSetViewport(current_buffer, 0, 1, &viewport);
	vkCmdSetScissor(current_buffer, 0, 1, &scissor);

	VkBuffer vertex_buffers[] = { vertex_buffer };
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindVertexBuffers(current_buffer, 0, 1, vertex_buffers, offsets);
	vkCmdBindIndexBuffer(current_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT16);

	VkPipeline pipeline = get_graphics_pipeline();
	VkDescriptorSet dset = get_descriptor_set();

	vkCmdBindDescriptorSets(
		current_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		m_pipeline_layout,
		0,
		1, &dset,
		1, &ubo_dynamic_offset
	);

	vkCmdBindPipeline(
		current_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline
	);

	vkCmdDrawIndexed(
		current_buffer,
		pass.mesh->index_count(),
		1,
		0,
		0,
		0
	);

	jjj++;
	if (jjj >= MAX_UBOS) {
		jjj = 0;
	}
}

void VulkanBackend::end_render()
{
	VkCommandBuffer current_buffer = current_frame().command_buffer;

	vkCmdEndRenderPass(current_buffer);

	if (VkResult result = vkEndCommandBuffer(current_buffer); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to record command buffer: %d", result);
	}
}

void VulkanBackend::swap_buffers()
{
	VkCommandBuffer current_buffer = current_frame().command_buffer;
	VkSemaphore wait_semaphore = current_frame().image_available_semaphore;
	VkSemaphore queue_finished_semaphore = current_frame().render_finished_semaphore;
	VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &wait_semaphore;
	submit_info.pWaitDstStageMask = &wait_stage;
	submit_info.pCommandBuffers = &current_buffer;
	submit_info.commandBufferCount = 1;
	submit_info.pSignalSemaphores = &queue_finished_semaphore;
	submit_info.signalSemaphoreCount = 1;

	vkResetFences(this->device, 1, &current_frame().in_flight_fence);

	if (VkResult result = vkQueueSubmit(queues.graphics, 1, &submit_info, current_frame().in_flight_fence); result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to submit draw command to buffer: %d", result);
	}

	wait_for_sync();

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &queue_finished_semaphore;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &m_swap_chain;
	present_info.pImageIndices = &m_curr_image_idx;
	present_info.pResults = nullptr;

	if (VkResult result = vkQueuePresentKHR(queues.present, &present_info); result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_is_framebuffer_resized) {
		m_is_framebuffer_resized = false;
		rebuild_swap_chain();
	} else if (result != VK_SUCCESS) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to present swap chain image: %d", result);
	}

	m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

	acquire_next_image();
}

void VulkanBackend::acquire_next_image()
{
	if (VkResult result = vkAcquireNextImageKHR(this->device, m_swap_chain, UINT64_MAX, current_frame().image_available_semaphore, VK_NULL_HANDLE, &m_curr_image_idx); result == VK_ERROR_OUT_OF_DATE_KHR) {
		rebuild_swap_chain();
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		WVN_ERROR("[VULKAN|DEBUG] Failed to acquire next image in swap chain: %d", result);
	}
}

void VulkanBackend::set_clear_colour(const Colour& colour)
{
	m_render_pass_builder.set_clear_colour(colour);
	m_render_pass_builder.set_depth_stencil_clear(1.0f, 0);
}

void VulkanBackend::set_depth_params(bool depth_test, bool depth_write)
{
	m_depth_stencil_create_info.depthTestEnable  = depth_test ? VK_TRUE : VK_FALSE;
	m_depth_stencil_create_info.depthWriteEnable = depth_write ? VK_TRUE : VK_FALSE;
}

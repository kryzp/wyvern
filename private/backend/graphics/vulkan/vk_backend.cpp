
// https://vulkan-tutorial.com
// Thank. You. So. Much.
// I love Vulkan. Thamk u Khronos (tm) (c) (r) uwu

#include <backend/graphics/vulkan/vk_backend.h>

#include <wvn/root.h>
#include <wvn/system/system_backend.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/graphics/render_pass.h>
#include <wvn/graphics/vertex.h>
#include <wvn/container/array.h>

#include <backend/graphics/vulkan/vk_shader.h>

/// debug ///
#include <wvn/io/file_stream.h>
/// debug ///

using namespace wvn;
using namespace wvn::gfx;

// TEMP //
static Vertex VERTICES[] = {
	{ { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
	{ {  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
	{ {  0.5f,  0.5f }, { 1.0f, 1.0f, 0.0f } },
	{ { -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } },
};

static u16 INDICES[] = {
	0, 1, 2, 2, 3, 0
};
// TEMP //

static const char* VALIDATION_LAYERS[] = {
	"VK_LAYER_KHRONOS_validation" // idk what the #define macro name for this is
};

static const char* DEVICE_EXTENSIONS[] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	"VK_KHR_portability_subset"
};

static VkDynamicState DYNAMIC_STATES[] = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR,
	VK_DYNAMIC_STATE_BLEND_CONSTANTS
};

static constexpr u32 MAX_FRAMES_IN_FLIGHT = 2;

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

			if (cstr::compare(layer_name_0, layer_name_1) == 0)
			{
				has_layer = true;
				break;
			}
		}

		if (!has_layer)
			return false;
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
		dev::LogMgr::get_singleton().print("[VULKAN:DEBUG] Validation Layer (SEVERITY: %d) (TYPE: %d): %s", messageSeverity, messageType, pCallbackData->pMessage);
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

	if (!Root::get_singleton().current_system_backend()->vk_get_instance_extensions(&ext_count, nullptr)) {
		WVN_ERROR("[VULKAN]: Unable to get instance extension count.");
	}

	Vector<const char*> extensions(ext_count);

	if (!Root::get_singleton().current_system_backend()->vk_get_instance_extensions(&ext_count, extensions.data())) {
		WVN_ERROR("[VULKAN]: Unable to get additional instance extensions.");
	}

#if WVN_DEBUG
	extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

	return extensions;
}

static Array<VkVertexInputAttributeDescription, 2> get_vertex_attribute_description()
{
	Array<VkVertexInputAttributeDescription, 2> result = {};

	// position part
	result[0].binding = 0;
	result[0].location = 0;
	result[0].format = VK_FORMAT_R32G32_SFLOAT;
	result[0].offset = offsetof(Vertex, pos);

	// colour part
	result[1].binding = 0;
	result[1].location = 1;
	result[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	result[1].offset = offsetof(Vertex, col);

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
	, m_command_pool(VK_NULL_HANDLE)
	, m_command_buffers()
	, m_staging_buffer()
	, m_vertex_buffer()
	, m_image_available_semaphores()
	, m_render_finished_semaphores()
	, m_in_flight_fences()
	, m_is_framebuffer_resized(false)
	, m_render_pass(VK_NULL_HANDLE)
	, m_pipeline_layout(VK_NULL_HANDLE)
	, m_graphics_pipeline(VK_NULL_HANDLE)
	, m_swap_chain(VK_NULL_HANDLE)
	, m_swap_chain_images()
	, m_swap_chain_image_views()
	, m_swap_chain_image_format()
	, m_swap_chain_extent()
	, m_swap_chain_framebuffers()
	, m_queues()
	, m_logical_data()
	, m_physical_data()
	, m_temp_shader(nullptr)
#if WVN_DEBUG
	, m_debug_messenger()
#endif
{
	VkApplicationInfo app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = Root::get_singleton().config().name,
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "Wyvern",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
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
		dev::LogMgr::get_singleton().print("[VULKAN:DEBUG] Validation layer support verified!");

		debug_populate_debug_utils_messenger_create_info_ext(&debug_create_info);

		create_info.enabledLayerCount = ARRAY_LENGTH(VALIDATION_LAYERS);
		create_info.ppEnabledLayerNames = VALIDATION_LAYERS;
		create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;

		g_debug_enable_validation_layers = true;
	}
	else
	{
		dev::LogMgr::get_singleton().print("[VULKAN:DEBUG] No validation layer support.");

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
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN] Failed to create instance.");
	}

#if WVN_DEBUG
	if (VkResult result = debug_create_debug_utils_messenger_ext(m_instance, &debug_create_info, nullptr, &m_debug_messenger); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN:DEBUG] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to create debug messenger");
	}
#endif

	// create surface
	if (bool result = Root::get_singleton().current_system_backend()->vk_create_surface(m_instance, &m_surface); !result) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to create surface.");
	}

	enumerate_physical_devices();

	QueueFamilyIdx phys_idx = find_queue_families(m_physical_data.device);

	create_logical_device(phys_idx);
	create_swap_chain(phys_idx);
	create_image_views();
	create_render_pass();
	create_graphics_pipeline();
	create_swap_chain_framebuffers();
	create_command_pool(phys_idx);
	create_vertex_buffer();
	create_index_buffer();
	create_command_buffers();
	create_sync_objects();

	dev::LogMgr::get_singleton().print("[VULKAN] Initialized!");
}

VulkanBackend::~VulkanBackend()
{
	vkDeviceWaitIdle(m_logical_data.device);

	// //

	clean_up_swap_chain();

	m_index_buffer.clean_up();
	m_vertex_buffer.clean_up();

	m_temp_shader->clean_up();

	vkDestroyPipeline(m_logical_data.device, m_graphics_pipeline, nullptr);
	vkDestroyPipelineLayout(m_logical_data.device, m_pipeline_layout, nullptr);
	vkDestroyRenderPass(m_logical_data.device, m_render_pass, nullptr);

	for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyFence(m_logical_data.device, m_in_flight_fences[i], nullptr);
		vkDestroySemaphore(m_logical_data.device, m_render_finished_semaphores[i], nullptr);
		vkDestroySemaphore(m_logical_data.device, m_image_available_semaphores[i], nullptr);
	}

	vkDestroyCommandPool(m_logical_data.device, m_command_pool, nullptr);
	vkDestroyDevice(m_logical_data.device, nullptr);

#if WVN_DEBUG
	if (g_debug_enable_validation_layers) {
		debug_destroy_debug_utils_messenger_ext(m_instance, m_debug_messenger, nullptr);
		dev::LogMgr::get_singleton().print("[VULKAN:DEBUG] Destroyed validation layers!");
	}
#endif

	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkDestroyInstance(m_instance, nullptr);

	dev::LogMgr::get_singleton().print("[VULKAN] Destroyed!");
}

RendererBackendProperties VulkanBackend::properties()
{
	RendererBackendProperties properties = {};
	properties.origin_bottom_left = false;

	return properties;
}

void VulkanBackend::enumerate_physical_devices()
{
	u32 device_count = 0;
	vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);

	if (!device_count) {
		WVN_ERROR("[VULKAN] Failed to find GPUs with Vulkan support!");
	}

	VkPhysicalDeviceProperties properties = {};
	VkPhysicalDeviceFeatures features = {};

	Vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

	vkGetPhysicalDeviceProperties(devices[0], &properties);
	vkGetPhysicalDeviceFeatures(devices[0], &features);

	m_physical_data.device = devices[0];
	m_physical_data.properties = properties;
	m_physical_data.features = features;

	bool has_essentials = false;
	u32 usability0 = assign_physical_device_usability(m_physical_data.device, properties, features, &has_essentials);

	// select the device of the highest usability
	for (int i = 1; i < device_count; i++)
	{
		vkGetPhysicalDeviceProperties(devices[i], &m_physical_data.properties);
		vkGetPhysicalDeviceFeatures(devices[i], &m_physical_data.features);

		u32 usability1 = assign_physical_device_usability(devices[i], properties, features, &has_essentials);

		if (usability1 > usability0 && has_essentials)
		{
			usability0 = usability1;

			m_physical_data.device = devices[i];
			m_physical_data.properties = properties;
			m_physical_data.features = features;
		}
	}

	if (m_physical_data.device == VK_NULL_HANDLE) {
		WVN_ERROR("[VULKAN] Unable to find a suitable GPU!");
	}

	dev::LogMgr::get_singleton().print("[VULKAN] Selected a suitable GPU: %d", m_physical_data.device);
}

void VulkanBackend::create_logical_device(const QueueFamilyIdx& phys_idx)
{
	constexpr float QUEUE_PRIORITY = 1.0f;

	Vector<VkDeviceQueueCreateInfo> queue_create_infos;

	for (auto family : phys_idx.package())
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
	create_info.pEnabledFeatures = &m_physical_data.features;

#if WVN_DEBUG
	if (g_debug_enable_validation_layers) {
		create_info.enabledLayerCount = ARRAY_LENGTH(VALIDATION_LAYERS);
		create_info.ppEnabledLayerNames = VALIDATION_LAYERS;
	}
	dev::LogMgr::get_singleton().print("[VULKAN:DEBUG] Enabled validation layers!");
#endif

	if (VkResult result = vkCreateDevice(m_physical_data.device, &create_info, nullptr, &m_logical_data.device); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to create logical device.");
	}

	vkGetDeviceQueue(m_logical_data.device, phys_idx.graphics_family.value(), 0, &m_queues.graphics_queue);
	vkGetDeviceQueue(m_logical_data.device, phys_idx.present_family.value(),  0, &m_queues.present_queue);
	vkGetDeviceQueue(m_logical_data.device, phys_idx.compute_family.value(),  0, &m_queues.compute_queue);
	vkGetDeviceQueue(m_logical_data.device, phys_idx.transfer_family.value(), 0, &m_queues.transfer_queue);

	dev::LogMgr::get_singleton().print("[VULKAN] Created a logical device!");
}

void VulkanBackend::create_swap_chain(const QueueFamilyIdx& phys_idx)
{
	SwapChainSupportDetails details = query_swap_chain_support(m_physical_data.device);

	auto surf_fmt  = choose_swap_surface_format(details.surface_formats);
	auto pres_mode = choose_swap_present_mode(details.present_modes);
	auto extent    = choose_swap_extent(details.capabilities);

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

	if (VkResult result = vkCreateSwapchainKHR(m_logical_data.device, &create_info, nullptr, &m_swap_chain); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to create swap chain.");
	}

	vkGetSwapchainImagesKHR(m_logical_data.device, m_swap_chain, &img_count, nullptr);

	if (!img_count) {
		WVN_ERROR("[VULKAN:DEBUG] Failed to find any images in swap chain!");
	}

	m_swap_chain_images.resize(img_count);
	vkGetSwapchainImagesKHR(m_logical_data.device, m_swap_chain, &img_count, m_swap_chain_images.data());

	this->m_swap_chain_image_format = surf_fmt.format;
	this->m_swap_chain_extent = extent;

	dev::LogMgr::get_singleton().print("[VULKAN] Created the swap chain!");
}

void VulkanBackend::create_image_views()
{
	m_swap_chain_image_views.resize(m_swap_chain_images.size());

	for (u64 i = 0; i < m_swap_chain_images.size(); i++)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = m_swap_chain_images[i];
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = m_swap_chain_image_format;

		create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		if (VkResult result = vkCreateImageView(m_logical_data.device, &create_info, nullptr, &m_swap_chain_image_views[i]); result != VK_SUCCESS) {
			dev::LogMgr::get_singleton().print("[VULKAN] Result [Index: %d]: %d", i, result);
			WVN_ERROR("[VULKAN:DEBUG] Failed to create image view.");
		}
	}

	dev::LogMgr::get_singleton().print("[VULKAN] Created image views!");
}

void VulkanBackend::create_graphics_pipeline()
{
	//////// debug /////////
	io::FileStream vert_fs = io::FileStream("/Users/kryzp/Documents/Projects/wyvern/test/res/vert.spv", "r");
	io::FileStream frag_fs = io::FileStream("/Users/kryzp/Documents/Projects/wyvern/test/res/frag.spv", "r");

	WVN_ASSERT(vert_fs.size() > 0, "Vertex file must not be empty.");
	WVN_ASSERT(frag_fs.size() > 0, "Fragment file must not be empty.");

	Vector<char> vert_source;
	Vector<char> frag_source;

	vert_source.resize(vert_fs.size());
	frag_source.resize(frag_fs.size());

	vert_fs.read(vert_source.data(), vert_fs.size());
	frag_fs.read(frag_source.data(), frag_fs.size());

	m_temp_shader = static_cast<VulkanShader*>(create_shader(vert_source, frag_source));
	//////// debug /////////

	auto binding_desc = get_vertex_binding_description();
	auto attribs_desc = get_vertex_attribute_description();

	VkPipelineShaderStageCreateInfo shader_stages[2] = { m_temp_shader->vert_info, m_temp_shader->frag_info };

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

	VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {};
	dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_create_info.dynamicStateCount = static_cast<u32>(ARRAY_LENGTH(DYNAMIC_STATES));
	dynamic_state_create_info.pDynamicStates = DYNAMIC_STATES;

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
	rasterization_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterization_state_create_info.depthBiasEnable = VK_FALSE;
	rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
	rasterization_state_create_info.depthBiasClamp = 0.0f;
	rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {};
	multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_state_create_info.sampleShadingEnable = VK_FALSE;
	multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisample_state_create_info.minSampleShading = 1.0f;
	multisample_state_create_info.pSampleMask = nullptr;
	multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
	multisample_state_create_info.alphaToOneEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {};

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

	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.setLayoutCount = 0;
	pipeline_layout_create_info.pSetLayouts = nullptr;
	pipeline_layout_create_info.pushConstantRangeCount = 0;
	pipeline_layout_create_info.pPushConstantRanges = nullptr;

	if (VkResult result = vkCreatePipelineLayout(m_logical_data.device, &pipeline_layout_create_info, nullptr, &m_pipeline_layout); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to create pipeline layout.");
	}

	VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {};
	graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphics_pipeline_create_info.stageCount = 2; // vert -> frag
	graphics_pipeline_create_info.pStages = shader_stages;
	graphics_pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
	graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
	graphics_pipeline_create_info.pViewportState = &viewport_state_create_info;
	graphics_pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
	graphics_pipeline_create_info.pMultisampleState = &multisample_state_create_info;
	graphics_pipeline_create_info.pDepthStencilState = &depth_stencil_state_create_info;
	graphics_pipeline_create_info.pColorBlendState = &colour_blend_state_create_info;
	graphics_pipeline_create_info.pDynamicState = &dynamic_state_create_info;
	graphics_pipeline_create_info.layout = m_pipeline_layout;
	graphics_pipeline_create_info.renderPass = m_render_pass;
	graphics_pipeline_create_info.subpass = 0;
	graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
	graphics_pipeline_create_info.basePipelineIndex = -1;

	if (VkResult result = vkCreateGraphicsPipelines(m_logical_data.device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &m_graphics_pipeline); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to create graphics pipeline.");
	}

	dev::LogMgr::get_singleton().print("[VULKAN] Created graphics pipeline!");
}

void VulkanBackend::create_render_pass()
{
	VkAttachmentDescription swap_chain_colour_attachment = {};
	swap_chain_colour_attachment.format = m_swap_chain_image_format;
	swap_chain_colour_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	swap_chain_colour_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	swap_chain_colour_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	swap_chain_colour_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // we dont use the stencil buffer YET
	swap_chain_colour_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	swap_chain_colour_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	swap_chain_colour_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference swap_chain_colour_attachment_ref = {};
	swap_chain_colour_attachment_ref.attachment = 0;
	swap_chain_colour_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription swap_chain_subpass = {};
	swap_chain_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	swap_chain_subpass.colorAttachmentCount = 1;
	swap_chain_subpass.pColorAttachments = &swap_chain_colour_attachment_ref;

	VkSubpassDependency subpass_dependency = {};
	subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpass_dependency.dstSubpass = 0;
	subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.srcAccessMask = 0;
	subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo render_pass_create_info = {};
	render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_create_info.attachmentCount = 1;
	render_pass_create_info.pAttachments = &swap_chain_colour_attachment;
	render_pass_create_info.subpassCount = 1;
	render_pass_create_info.pSubpasses = &swap_chain_subpass;
	render_pass_create_info.dependencyCount = 1;
	render_pass_create_info.pDependencies = &subpass_dependency;

	if (VkResult result = vkCreateRenderPass(m_logical_data.device, &render_pass_create_info, nullptr, &m_render_pass); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to create render pass.");
	}

	dev::LogMgr::get_singleton().print("[VULKAN] Created render pass!");
}

void VulkanBackend::create_swap_chain_framebuffers()
{
	m_swap_chain_framebuffers.resize(m_swap_chain_image_views.size());

	for (u64 i = 0; i < m_swap_chain_image_views.size(); i++)
	{
		VkImageView* attachments = &m_swap_chain_image_views[i];

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_render_pass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_swap_chain_extent.width;
		framebufferInfo.height = m_swap_chain_extent.height;
		framebufferInfo.layers = 1;

		if (VkResult result = vkCreateFramebuffer(m_logical_data.device, &framebufferInfo, nullptr, &m_swap_chain_framebuffers[i]); result != VK_SUCCESS) {
			dev::LogMgr::get_singleton().print("[VULKAN] Result [Index: %d]: %d", i, result);
			WVN_ERROR("[VULKAN:DEBUG] Failed to create framebuffer.");
		}
	}

	dev::LogMgr::get_singleton().print("[VULKAN] Created swap chain framebuffers!");
}

void VulkanBackend::clean_up_swap_chain()
{
	for (auto& fbo : m_swap_chain_framebuffers) {
		vkDestroyFramebuffer(m_logical_data.device, fbo, nullptr);
	}

	for (auto& view : m_swap_chain_image_views) {
		vkDestroyImageView(m_logical_data.device, view, nullptr);
	}

	vkDestroySwapchainKHR(m_logical_data.device, m_swap_chain, nullptr);
}

void VulkanBackend::rebuild_swap_chain()
{
	// wait until draw size isnt zero
	while (Root::get_singleton().current_system_backend()->get_draw_size() == Vec2I::zero()) { }

	vkDeviceWaitIdle(m_logical_data.device);
	clean_up_swap_chain();

	QueueFamilyIdx phys_idx = find_queue_families(m_physical_data.device);

	create_swap_chain(phys_idx);
	create_image_views();
	create_swap_chain_framebuffers();
}

void VulkanBackend::create_command_pool(const QueueFamilyIdx& phys_idx)
{
	VkCommandPoolCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	create_info.queueFamilyIndex = phys_idx.graphics_family.value();

	if (VkResult result = vkCreateCommandPool(m_logical_data.device, &create_info, nullptr, &m_command_pool); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to create command pool.");
	}

	dev::LogMgr::get_singleton().print("[VULKAN] Created command pool!");
}

void VulkanBackend::create_command_buffers()
{
	m_command_buffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool = m_command_pool;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

	if (VkResult result = vkAllocateCommandBuffers(m_logical_data.device, &command_buffer_allocate_info, m_command_buffers.data()); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to create command buffer.");
	}

	dev::LogMgr::get_singleton().print("[VULKAN] Created command buffer!");
}

void VulkanBackend::create_sync_objects()
{
	m_image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_in_flight_fences          .resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (VkResult result = vkCreateSemaphore(m_logical_data.device, &semaphore_create_info, nullptr, &m_image_available_semaphores[i]); result != VK_SUCCESS) {
			dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
			WVN_ERROR("[VULKAN:DEBUG] Failed to create image available semaphore.");
		}

		if (VkResult result = vkCreateSemaphore(m_logical_data.device, &semaphore_create_info, nullptr, &m_render_finished_semaphores[i]); result != VK_SUCCESS) {
			dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
			WVN_ERROR("[VULKAN:DEBUG] Failed to create render finished semaphore.");
		}

		if (VkResult result = vkCreateFence(m_logical_data.device, &fence_create_info, nullptr, &m_in_flight_fences[i]); result != VK_SUCCESS) {
			dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
			WVN_ERROR("[VULKAN:DEBUG] Failed to create in flight fence.");
		}
	}

	dev::LogMgr::get_singleton().print("[VULKAN] Created sync objects!");
}

void VulkanBackend::create_vertex_buffer()
{
	VkDeviceSize buffer_size = sizeof(VERTICES[0]) * ARRAY_LENGTH(VERTICES);

	m_staging_buffer.create(
		m_logical_data.device, m_physical_data.device,
		buffer_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	m_staging_buffer.send_data(VERTICES);

	m_vertex_buffer.create(
		m_logical_data.device, m_physical_data.device,
		buffer_size,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	m_staging_buffer.copy_to(m_vertex_buffer, m_command_pool, m_queues.graphics_queue);
	m_staging_buffer.clean_up();

	dev::LogMgr::get_singleton().print("[VULKAN] Created vertex buffer!");
}

void VulkanBackend::create_index_buffer()
{
	VkDeviceSize buffer_size = sizeof(INDICES[0]) * ARRAY_LENGTH(INDICES);

	m_staging_buffer.create(
		m_logical_data.device, m_physical_data.device,
		buffer_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	m_staging_buffer.send_data(INDICES);

	m_index_buffer.create(
		m_logical_data.device, m_physical_data.device,
		buffer_size,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	m_staging_buffer.copy_to(m_index_buffer, m_command_pool, m_queues.graphics_queue);
	m_staging_buffer.clean_up();

	dev::LogMgr::get_singleton().print("[VULKAN] Created index buffer!");
}

// abstract function that generates a "usability" or "goodness value" of a given device
u32 VulkanBackend::assign_physical_device_usability(
	VkPhysicalDevice device,
	VkPhysicalDeviceProperties properties,
	VkPhysicalDeviceFeatures features,
	bool* essentials_completed
)
{
	u32 result = 0;

	bool adequate_swap_chain = false;
	bool has_required_extensions = check_device_extension_support(device);
	bool indices_complete = find_queue_families(device).is_complete();

	if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		result += 2;
	} else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
		result += 1;
	}

	if (features.geometryShader) {
		result += 1;
	}

	if (has_required_extensions) {
		SwapChainSupportDetails swap_chain_support_details = query_swap_chain_support(device);
		adequate_swap_chain = swap_chain_support_details.surface_formats.any() && swap_chain_support_details.present_modes.any();
	}

	if (essentials_completed) {
		(*essentials_completed) = indices_complete && has_required_extensions && adequate_swap_chain;
	}

	return result;
}

QueueFamilyIdx VulkanBackend::find_queue_families(VkPhysicalDevice device)
{
	QueueFamilyIdx result;

	u32 queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

	if (!queue_family_count) {
		WVN_ERROR("[VULKAN] Failed to find any queue families!");
	}

	Vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

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
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &present_support);

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

bool VulkanBackend::check_device_extension_support(VkPhysicalDevice device)
{
	u32 ext_count = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &ext_count, nullptr);

	if (!ext_count) {
		WVN_ERROR("[VULKAN] Failed to find any device extension properties!");
	}

	Vector<VkExtensionProperties> available_exts(ext_count);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &ext_count, available_exts.data());

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

SwapChainSupportDetails VulkanBackend::query_swap_chain_support(VkPhysicalDevice device)
{
	SwapChainSupportDetails result = {};

	// get capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &result.capabilities);

	// get surface formats
	u32 surf_fmt_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &surf_fmt_count, nullptr);

	if (surf_fmt_count)
	{
		result.surface_formats.resize(surf_fmt_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &surf_fmt_count, result.surface_formats.data());
	}

	// get present modes
	u32 present_mode_count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &present_mode_count, nullptr);

	if (present_mode_count)
	{
		result.present_modes.resize(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &present_mode_count, result.present_modes.data());
	}

	return result;
}

VkSurfaceFormatKHR VulkanBackend::choose_swap_surface_format(const Vector<VkSurfaceFormatKHR>& available_surface_formats)
{
	// todo: this could be ranked based on how good each format is rather than choosing the first one that fits

	for (auto& available_format : available_surface_formats)
	{
		if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return available_format;
		}
	}

	return available_surface_formats[0];
}

VkPresentModeKHR VulkanBackend::choose_swap_present_mode(const Vector<VkPresentModeKHR>& available_present_modes)
{
	for (const auto& available_present_mode : available_present_modes)
	{
		if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return available_present_mode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanBackend::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != CalcU::max_number())
	{
		return capabilities.currentExtent;
	}
	else
	{
		Vec2I wh = Root::get_singleton().current_system_backend()->get_window_size();
		VkExtent2D actual_extent = { static_cast<u32>(wh.w), static_cast<u32>(wh.h) };

		actual_extent.width  = CalcU::clamp(actual_extent.width,  capabilities.minImageExtent.width,  capabilities.maxImageExtent.width );
		actual_extent.height = CalcU::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actual_extent;
	}
}

VkShaderModule VulkanBackend::create_shader_module(const Vector<char>& source)
{
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = source.size();
	create_info.pCode = reinterpret_cast<const u32*>(source.data());

	VkShaderModule module = {};

	if (VkResult result = vkCreateShaderModule(m_logical_data.device, &create_info, nullptr, &module); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to create shader module.");
	}

	dev::LogMgr::get_singleton().print("[VULKAN] Created shader module!");

	return module;
}

Texture* VulkanBackend::create_texture(u32 width, u32 height)
{
	return nullptr;
}

Shader* VulkanBackend::create_shader(const Vector<char>& vert_source, const Vector<char>& frag_source)
{
	VkShaderModule vert_module = create_shader_module(vert_source);
	VkShaderModule frag_module = create_shader_module(frag_source);

	return new VulkanShader(vert_module, frag_module, m_logical_data.device);
}

RenderTarget* VulkanBackend::create_render_target(u32 width, u32 height)
{
	return nullptr;
}

Mesh* VulkanBackend::create_mesh()
{
	return nullptr;
}

void VulkanBackend::wait_for_sync()
{
	vkWaitForFences(m_logical_data.device, 1, &m_in_flight_fences[m_current_frame], VK_TRUE, UINT64_MAX);
}

void VulkanBackend::on_window_resize(int width, int height)
{
	m_is_framebuffer_resized = true;
}

void VulkanBackend::render(const RenderPass& pass)
{
	u32 img_idx;

	if (VkResult result = vkAcquireNextImageKHR(m_logical_data.device, m_swap_chain, UINT64_MAX, m_image_available_semaphores[m_current_frame], VK_NULL_HANDLE, &img_idx); result == VK_ERROR_OUT_OF_DATE_KHR) {
		rebuild_swap_chain();
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to acquire next image in swap chain.");
	}

	VkCommandBuffer& current_buffer = m_command_buffers[m_current_frame];

	vkResetFences(m_logical_data.device, 1, &m_in_flight_fences[m_current_frame]);
	vkResetCommandBuffer(current_buffer, 0);

	// render pass stuff
	{
		VkCommandBufferBeginInfo command_buffer_begin_info = {};
		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		command_buffer_begin_info.flags = 0;
		command_buffer_begin_info.pInheritanceInfo = nullptr;

		if (VkResult result = vkBeginCommandBuffer(current_buffer, &command_buffer_begin_info); result != VK_SUCCESS) {
			dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
			WVN_ERROR("[VULKAN:DEBUG] Failed to begin recording command buffer.");
		}

		VkClearValue clear_colour = {};
		pass.clear_colour.premultiplied().export_to_float(clear_colour.color.float32);

		VkRenderPassBeginInfo render_pass_begin_info = {};
		render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_begin_info.renderPass = m_render_pass;
		render_pass_begin_info.framebuffer = m_swap_chain_framebuffers[img_idx];
		render_pass_begin_info.renderArea.offset = { 0, 0 };
		render_pass_begin_info.renderArea.extent = m_swap_chain_extent;
		render_pass_begin_info.clearValueCount = 1;
		render_pass_begin_info.pClearValues = &clear_colour;

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
			viewport.width = static_cast<float>(m_swap_chain_extent.width);
			viewport.height = static_cast<float>(m_swap_chain_extent.height);
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

		vkCmdBeginRenderPass(current_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
		{
			vkCmdBindPipeline(current_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);

			vkCmdSetViewport(current_buffer, 0, 1, &viewport);
			vkCmdSetScissor(current_buffer, 0, 1, &scissor);

			VkBuffer vertex_buffers[] = { m_vertex_buffer.buffer() };
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(current_buffer, 0, 1, vertex_buffers, offsets);
			vkCmdBindIndexBuffer(current_buffer, m_index_buffer.buffer(), 0, VK_INDEX_TYPE_UINT16);

			vkCmdDrawIndexed(current_buffer, ARRAY_LENGTH(INDICES), 1, 0, 0, 0);
		}
		vkCmdEndRenderPass(current_buffer);

		if (VkResult result = vkEndCommandBuffer(current_buffer); result != VK_SUCCESS) {
			dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
			WVN_ERROR("[VULKAN:DEBUG] Failed to record command buffer.");
		}
	}

	VkSemaphore wait_semaphores[] = { m_image_available_semaphores[m_current_frame]};
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signal_semaphores[] = { m_render_finished_semaphores[m_current_frame] };

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &current_buffer;
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	if (VkResult result = vkQueueSubmit(m_queues.graphics_queue, 1, &submit_info, m_in_flight_fences[m_current_frame]); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("Failed to submit draw command to buffer.");
	}

	VkSwapchainKHR swapchains[] = { m_swap_chain };

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;
	present_info.pImageIndices = &img_idx;
	present_info.pResults = nullptr;

	if (VkResult result = vkQueuePresentKHR(m_queues.present_queue, &present_info); result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_is_framebuffer_resized) {
		m_is_framebuffer_resized = false;
		rebuild_swap_chain();
	} else if (result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to present swap chain image.");
	}

	m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

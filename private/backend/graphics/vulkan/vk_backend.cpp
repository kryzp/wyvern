#include <backend/graphics/vulkan/vk_backend.h>
#include <backend/graphics/vulkan/vk_util.h>
#include <backend/graphics/vulkan/vk_texture.h>
#include <backend/graphics/vulkan/vk_backbuffer.h>

#include <wvn/root.h>
#include <wvn/system/system_backend.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/graphics/render_pass.h>
#include <wvn/graphics/vertex.h>
#include <wvn/container/array.h>
#include <wvn/maths/vec3.h>

/* =====================
 * TODO LIST
 * ---------------------
 * [ ] INSTANCED RENDERING
 * [ ] COMPUTE SHADERS
 * [ ] MULTIPLE SUBPASSES
 * [ ] MULTITHREADED COMMAND BUFFER GENERATION
 * [x] SEPARATE IMAGES & SAMPLER DESCRIPTORS
 * [x] PUSH CONSTANTS
 * ===================== */

/*
 * Note to self.
 * Resizing the uniform buffer is very volatile right now!
 * IF it is resized during the rendering of a mesh, and it runs out of memory again DURING THE SAME FRAME
 * when rendering another mesh, it WILL CRASH. This is because the command buffer HAS NOT BEEN NOTIFIED of the
 * change in uniform buffer until the end of the frame, so rendering will be using an INVALID COMMAND BUFFER!!!
 */

using namespace wvn;
using namespace wvn::gfx;

static VkDynamicState DYNAMIC_STATES[] = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR,
	VK_DYNAMIC_STATE_BLEND_CONSTANTS
};

#if wvn_DEBUG

static bool g_debug_enable_validation_layers = false;

static bool debug_has_validation_layer_support()
{
	u32 layer_count = 0;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	VkLayerProperties available_layers[layer_count];
	vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

	for (int i = 0; i < wvn_ARRAY_LENGTH(vkutil::VALIDATION_LAYERS); i++)
	{
		bool has_layer = false;
		const char* layer_name_0 = vkutil::VALIDATION_LAYERS[i];

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
		wvn_ERROR("[VULKAN|DEBUG] Validation Layer (SEVERITY: %d) (TYPE: %d): %s", messageSeverity, messageType, pCallbackData->pMessage);
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

#endif // wvn_DEBUG

static Vector<const char*> get_instance_extensions()
{
	u32 ext_count = 0;
	const char* const* names = Root::get_singleton()->system_backend()->vk_get_instance_extensions(&ext_count);

	if (!names) {
		wvn_ERROR("[VULKAN|DEBUG] Unable to get instance extension count.");
	}

	Vector<const char*> extensions(ext_count);

	for (int i = 0; i < ext_count; i++) {
		extensions[i] = names[i];
	}

#if wvn_DEBUG
	extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // wvn_DEBUG

	extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

	return extensions;
}

static Array<VkVertexInputAttributeDescription, 4> get_vertex_attribute_description()
{
	Array<VkVertexInputAttributeDescription, 4> result = {};

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

	// normal part
	result[3].binding = 0;
	result[3].location = 3;
	result[3].format = VK_FORMAT_R32G32B32_SFLOAT;
	result[3].offset = offsetof(Vertex, norm);

	return result;
}

static VkVertexInputBindingDescription get_vertex_binding_description()
{
	return {
		.binding = 0,
		.stride = sizeof(Vertex),
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};
}

RendererBackendProperties VulkanBackend::properties() { return {
	.y_positive_down = true
}; }

VulkanBackend::VulkanBackend()
	: vulkan_instance(VK_NULL_HANDLE)
	, m_current_frame_idx(0)
	, m_current_render_pass_builder()
	, m_descriptor_pool_mgr(this)
	, m_image_infos()
	, m_shader_stages()
	, m_sample_shading_enabled(true)
	, m_ubo_mgr()
	, m_push_constants()
	, m_min_sample_shading(0.2f)
	, m_blend_state_logic_op_enabled(false)
	, m_blend_state_logic_op(VK_LOGIC_OP_COPY)
	, m_pipeline_cache()
	, m_pipeline_layout_cache()
	, m_descriptor_cache(this)
	, m_descriptor_builder()
	, m_descriptor_builder_dirty(true)
	, m_pipeline_process_cache()
	, swap_chain_image_format()
	, m_texture_mgr(nullptr)
	, m_shader_mgr(nullptr)
	, m_buffer_mgr(nullptr)
	, m_render_target_mgr(nullptr)
	, frames()
	, queues()
	, device(VK_NULL_HANDLE)
	, physical_data()
	, m_depth_stencil_create_info()
	, m_colour_blend_attachment_state()
	, m_blend_constants{0.0f, 0.0f, 0.0f, 0.0f}
	, msaa_samples(VK_SAMPLE_COUNT_1_BIT)
	, m_viewport()
	, m_scissor()
	, m_cull_mode(VK_CULL_MODE_BACK_BIT)
//	, m_current_shader_parameters()
#if wvn_DEBUG
	, m_debug_messenger()
#endif // wvn_DEBUG
{
	VkApplicationInfo app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = Root::get_singleton()->config().name,
		.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
		.pEngineName = "Raven Engine",
		.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
		.apiVersion = VK_API_VERSION_1_3
	};

	VkInstanceCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app_info
	};

#if wvn_DEBUG
	VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};

	if (debug_has_validation_layer_support())
	{
		dev::LogMgr::get_singleton()->print("[VULKAN|DEBUG] Validation layer support verified.");

		debug_populate_debug_utils_messenger_create_info_ext(&debug_create_info);

		create_info.enabledLayerCount = wvn_ARRAY_LENGTH(vkutil::VALIDATION_LAYERS);
		create_info.ppEnabledLayerNames = vkutil::VALIDATION_LAYERS;
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
#endif // wvn_DEBUG

	auto extensions = get_instance_extensions();
	create_info.enabledExtensionCount = extensions.size();
	create_info.ppEnabledExtensionNames = extensions.data();
	create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

	if (VkResult result = vkCreateInstance(&create_info, nullptr, &vulkan_instance); result != VK_SUCCESS) {
		wvn_ERROR("[VULKAN|DEBUG] Failed to create instance: %d", result);
	}

#if wvn_DEBUG
	if (VkResult result = debug_create_debug_utils_messenger_ext(vulkan_instance, &debug_create_info, nullptr, &m_debug_messenger); result != VK_SUCCESS) {
		wvn_ERROR("[VULKAN|DEBUG] Failed to create debug messenger: %d", result);
	}
#endif // wvn_DEBUG

	m_buffer_mgr        = new VulkanBufferMgr (this);
	m_texture_mgr       = new VulkanTextureMgr(this);
	m_shader_mgr        = new VulkanShaderMgr (this);
	m_render_target_mgr = new VulkanRenderTargetMgr(this);

	// finished :D
	dev::LogMgr::get_singleton()->print("[VULKAN] Initialized!");
}

VulkanBackend::~VulkanBackend()
{
	sync_stall();

	// //

	m_backbuffer->clean_up_swap_chain();

	delete m_render_target_mgr;
	delete m_shader_mgr;
	delete m_texture_mgr;
	delete m_buffer_mgr;

	m_backbuffer->clean_up_textures();

	clear_pipeline_cache();
	vkDestroyPipelineCache(this->device, m_pipeline_process_cache, nullptr);

	m_current_render_pass_builder->clean_up();

	m_ubo_mgr.clean_up();

	m_descriptor_cache.clean_up();
	m_descriptor_pool_mgr.clean_up();

	for (int i = 0; i < vkutil::FRAMES_IN_FLIGHT; i++) {
		vkDestroyFence(this->device, frames[i].in_flight_fence, nullptr);
		vkDestroyCommandPool(this->device, frames[i].command_pool, nullptr);
	}

	delete m_backbuffer;

	vkDestroyDevice(this->device, nullptr);

#if wvn_DEBUG
	if (g_debug_enable_validation_layers) {
		debug_destroy_debug_utils_messenger_ext(vulkan_instance, m_debug_messenger, nullptr);
		dev::LogMgr::get_singleton()->print("[VULKAN|DEBUG] Destroyed validation layers!");
	}
#endif // wvn_DEBUG

	vkDestroyInstance(vulkan_instance, nullptr);

	// //

	dev::LogMgr::get_singleton()->print("[VULKAN] Destroyed!");
}

void VulkanBackend::enumerate_physical_devices()
{
	VkSurfaceKHR surface = m_backbuffer->get_surface();

	u32 device_count = 0;
	vkEnumeratePhysicalDevices(vulkan_instance, &device_count, nullptr);

	if (!device_count) {
		wvn_ERROR("[VULKAN|DEBUG] Failed to find GPUs with Vulkan support!");
	}

	VkPhysicalDeviceProperties properties = {};
	VkPhysicalDeviceFeatures features = {};

	Vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(vulkan_instance, &device_count, devices.data());

	vkGetPhysicalDeviceProperties(devices[0], &properties);
	vkGetPhysicalDeviceFeatures(devices[0], &features);

	physical_data.device = devices[0];
	physical_data.properties = properties;
	physical_data.features = features;

	this->msaa_samples = get_max_usable_sample_count();

	bool has_essentials = false;
	u32 usability0 = vkutil::assign_physical_device_usability(surface, physical_data.device, properties, features, &has_essentials);

	// select the device of the highest usability
	for (int i = 1; i < device_count; i++)
	{
		vkGetPhysicalDeviceProperties(devices[i], &physical_data.properties);
		vkGetPhysicalDeviceFeatures(devices[i], &physical_data.features);

		u32 usability1 = vkutil::assign_physical_device_usability(surface, devices[i], properties, features, &has_essentials);

		if (usability1 > usability0 && has_essentials)
		{
			usability0 = usability1;

			physical_data.device = devices[i];
			physical_data.properties = properties;
			physical_data.features = features;

			this->msaa_samples = get_max_usable_sample_count();
		}
	}

	if (physical_data.device == VK_NULL_HANDLE) {
		wvn_ERROR("[VULKAN|DEBUG] Unable to find a suitable GPU!");
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Selected a suitable GPU: %d", physical_data.device);
}

void VulkanBackend::create_logical_device(const QueueFamilyIdx& phys_idx)
{
	const float QUEUE_PRIORITY = 1.0f;

	Vector<VkDeviceQueueCreateInfo> queue_create_infos;

	for (auto& family : phys_idx.package())
	{
		if (family == -1) {
			continue;
		}

		queue_create_infos.push_back({
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = family,
			.queueCount = 1,
			.pQueuePriorities = &QUEUE_PRIORITY
		});
	}

	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.queueCreateInfoCount = queue_create_infos.size();
	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.enabledLayerCount = 0;
	create_info.ppEnabledLayerNames = nullptr;
	create_info.ppEnabledExtensionNames = vkutil::DEVICE_EXTENSIONS;
	create_info.enabledExtensionCount = wvn_ARRAY_LENGTH(vkutil::DEVICE_EXTENSIONS);
	create_info.pEnabledFeatures = &physical_data.features;

#if wvn_DEBUG
	if (g_debug_enable_validation_layers) {
		create_info.enabledLayerCount = wvn_ARRAY_LENGTH(vkutil::VALIDATION_LAYERS);
		create_info.ppEnabledLayerNames = vkutil::VALIDATION_LAYERS;
	}
	dev::LogMgr::get_singleton()->print("[VULKAN|DEBUG] Enabled validation layers!");
#endif // wvn_DEBUG

	if (VkResult result = vkCreateDevice(physical_data.device, &create_info, nullptr, &this->device); result != VK_SUCCESS) {
		wvn_ERROR("[VULKAN|DEBUG] Failed to create logical device: %d", result);
	}

	vkGetDeviceQueue(this->device, phys_idx.graphics_family.value(), 0, &queues.graphics);
	vkGetDeviceQueue(this->device, phys_idx.present_family.value(),  0, &queues.present);
	vkGetDeviceQueue(this->device, phys_idx.compute_family.value(),  0, &queues.compute);
	vkGetDeviceQueue(this->device, phys_idx.transfer_family.value(), 0, &queues.transfer);

	dev::LogMgr::get_singleton()->print("[VULKAN] Created a logical device!");
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
		wvn_ERROR("[VULKAN|DEBUG] Failed to process pipeline cache: %d", result);
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created graphics pipeline process cache!");
}

VkPipelineLayout VulkanBackend::get_graphics_pipeline_layout()
{
	VulkanDescriptorBuilder builder = get_descriptor_builder();
	u64 pipeline_layout_hash = builder.hash();

	u64 push_constant_count = m_push_constants.size();
	hash::combine(&pipeline_layout_hash, &push_constant_count);

	if (m_pipeline_layout_cache.contains(pipeline_layout_hash)) {
		return m_pipeline_layout_cache[pipeline_layout_hash];
	}

	VkDescriptorSetLayout layout = {};
	builder.build_layout(layout);

	VkPushConstantRange push_constants;
	push_constants.offset = 0;
	push_constants.size = m_push_constants.size();
	push_constants.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;

	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.setLayoutCount = 1;
	pipeline_layout_create_info.pSetLayouts = &layout;
	pipeline_layout_create_info.pushConstantRangeCount = 1;
	pipeline_layout_create_info.pPushConstantRanges = &push_constants;

	VkPipelineLayout pipeline_layout = {};

	if (VkResult result = vkCreatePipelineLayout(this->device, &pipeline_layout_create_info, nullptr, &pipeline_layout); result != VK_SUCCESS) {
		wvn_ERROR("[VULKAN|DEBUG] Failed to create pipeline layout: %d", result);
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created new graphics pipeline layout!");

	m_pipeline_layout_cache.insert(Pair(
		pipeline_layout_hash,
		pipeline_layout
	));

	return pipeline_layout;
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

	// todo: wtf?
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = m_viewport.width;
	viewport.height = m_viewport.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = { (u32)m_viewport.width, (u32)m_viewport.height };

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
	rasterization_state_create_info.cullMode = m_cull_mode;
	rasterization_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterization_state_create_info.depthBiasEnable = VK_FALSE;
	rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
	rasterization_state_create_info.depthBiasClamp = 0.0f;
	rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {};
	multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_state_create_info.sampleShadingEnable = m_sample_shading_enabled ? VK_TRUE : VK_FALSE;
	multisample_state_create_info.minSampleShading = m_min_sample_shading;
	multisample_state_create_info.rasterizationSamples = (VkSampleCountFlagBits)m_current_render_target->get_msaa();
	multisample_state_create_info.pSampleMask = nullptr;
	multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
	multisample_state_create_info.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colour_blend_state_create_info = {};
	colour_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colour_blend_state_create_info.logicOpEnable = m_blend_state_logic_op_enabled ? VK_TRUE : VK_FALSE;
	colour_blend_state_create_info.logicOp = m_blend_state_logic_op;
	colour_blend_state_create_info.attachmentCount = 1;
	colour_blend_state_create_info.pAttachments = &m_colour_blend_attachment_state;
	colour_blend_state_create_info.blendConstants[0] = m_blend_constants[0];
	colour_blend_state_create_info.blendConstants[1] = m_blend_constants[1];
	colour_blend_state_create_info.blendConstants[2] = m_blend_constants[2];
	colour_blend_state_create_info.blendConstants[3] = m_blend_constants[3];

	u64 created_pipeline_hash = 0;

	hash::combine(&created_pipeline_hash, &m_depth_stencil_create_info);
	hash::combine(&created_pipeline_hash, &m_colour_blend_attachment_state);
	hash::combine(&created_pipeline_hash, &m_blend_constants);
	hash::combine(&created_pipeline_hash, &rasterization_state_create_info);
	hash::combine(&created_pipeline_hash, &input_assembly_state_create_info);
	hash::combine(&created_pipeline_hash, &multisample_state_create_info);

	VkRenderPass pass = m_current_render_pass_builder->get_render_pass();
	hash::combine(&created_pipeline_hash, &pass);

	Array<VkVertexInputAttributeDescription, 4> attrib_desctiption = get_vertex_attribute_description();
	VkVertexInputBindingDescription binding_description = get_vertex_binding_description();

	for (int i = 0; i < attrib_desctiption.size(); i++) {
		hash::combine(&created_pipeline_hash, &attrib_desctiption[i]);
	}

	hash::combine(&created_pipeline_hash, &binding_description);

	for (int i = 0; i < m_shader_stages.size(); i++) {
		hash::combine(&created_pipeline_hash, &m_shader_stages[i]);
	}

	if (m_pipeline_cache.contains(created_pipeline_hash)) {
		return m_pipeline_cache[created_pipeline_hash];
	}

	VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {};
	dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_create_info.dynamicStateCount = wvn_ARRAY_LENGTH(DYNAMIC_STATES);
	dynamic_state_create_info.pDynamicStates = DYNAMIC_STATES;

	VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {};
	graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphics_pipeline_create_info.pStages = m_shader_stages.data();
	graphics_pipeline_create_info.stageCount = m_shader_stages.size();
	graphics_pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
	graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
	graphics_pipeline_create_info.pViewportState = &viewport_state_create_info;
	graphics_pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
	graphics_pipeline_create_info.pMultisampleState = &multisample_state_create_info;
	graphics_pipeline_create_info.pDepthStencilState = &m_depth_stencil_create_info;
	graphics_pipeline_create_info.pColorBlendState = &colour_blend_state_create_info;
	graphics_pipeline_create_info.pDynamicState = &dynamic_state_create_info;
	graphics_pipeline_create_info.layout = get_graphics_pipeline_layout();
	graphics_pipeline_create_info.renderPass = m_current_render_pass_builder->get_render_pass();
	graphics_pipeline_create_info.subpass = 0;
	graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
	graphics_pipeline_create_info.basePipelineIndex = -1;

	VkPipeline created_pipeline = VK_NULL_HANDLE;

	if (VkResult result = vkCreateGraphicsPipelines(this->device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &created_pipeline); result != VK_SUCCESS) {
		wvn_ERROR("[VULKAN|DEBUG] Failed to create new graphics pipeline: %d", result);
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created new graphics pipeline!");

	m_pipeline_cache.insert(Pair(
		created_pipeline_hash,
		created_pipeline
	));

	return created_pipeline;
}

void VulkanBackend::create_command_pools(u32 graphics_family_idx)
{
	VkCommandPoolCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	create_info.queueFamilyIndex = graphics_family_idx;

	for (int i = 0; i < vkutil::FRAMES_IN_FLIGHT; i++) {
		if (VkResult result = vkCreateCommandPool(this->device, &create_info, nullptr, &frames[i].command_pool); result != VK_SUCCESS) {
			wvn_ERROR("[VULKAN|DEBUG] Failed to create command pools: %d", result);
		}
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created command pool!");
}

void VulkanBackend::create_command_buffers()
{
	for (int i = 0; i < vkutil::FRAMES_IN_FLIGHT; i++)
	{
		VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
		command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_allocate_info.commandPool = frames[i].command_pool;
		command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_allocate_info.commandBufferCount = 1;

		if (VkResult result = vkAllocateCommandBuffers(this->device, &command_buffer_allocate_info, &frames[i].command_buffer); result != VK_SUCCESS) {
			wvn_ERROR("[VULKAN|DEBUG] Failed to create command buffers: %d", result);
		}
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created command buffer!");
}

Backbuffer* VulkanBackend::create_backbuffer()
{
	VulkanBackbuffer* backbuffer = new VulkanBackbuffer(this);
	backbuffer->create_surface();

	m_backbuffer = backbuffer;
	set_render_target(backbuffer);

	enumerate_physical_devices();

	m_depth_stencil_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	m_depth_stencil_create_info.depthTestEnable = VK_TRUE;
	m_depth_stencil_create_info.depthWriteEnable = VK_TRUE;
	m_depth_stencil_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
	m_depth_stencil_create_info.depthBoundsTestEnable = VK_FALSE;
	m_depth_stencil_create_info.minDepthBounds = 0.0f;
	m_depth_stencil_create_info.maxDepthBounds = 1.0f;
	m_depth_stencil_create_info.stencilTestEnable = VK_FALSE;
	m_depth_stencil_create_info.front = {};
	m_depth_stencil_create_info.back = {};

	m_colour_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	m_colour_blend_attachment_state.blendEnable = VK_TRUE;
	m_colour_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	m_colour_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	m_colour_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
	m_colour_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	m_colour_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	m_colour_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

	QueueFamilyIdx queue_families = vkutil::find_queue_families(this->physical_data.device, m_backbuffer->get_surface());

	create_logical_device(queue_families);
//	get_graphics_pipeline_layout();
	create_pipeline_process_cache();
	create_command_pools(queue_families.graphics_family.value());
	create_command_buffers();

	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (int i = 0; i < vkutil::FRAMES_IN_FLIGHT; i++) {
		if (VkResult result = vkCreateFence(this->device, &fence_create_info, nullptr, &frames[i].in_flight_fence); result != VK_SUCCESS) {
			wvn_ERROR("[VULKAN|DEBUG] Failed to create in flight fence: %d", result);
		}
	}

	dev::LogMgr::get_singleton()->print("[VULKAN] Created in flight fence!");

	m_ubo_mgr.init(this,
		1024 * 256 * vkutil::FRAMES_IN_FLIGHT
	);

	m_descriptor_pool_mgr.init();

	backbuffer->create();

	set_render_target(m_backbuffer);

	m_viewport.x = 0.0f;
	m_viewport.y = 0.0f;
	m_viewport.width = (float)m_current_render_pass_builder->get_width();
	m_viewport.height = (float)m_current_render_pass_builder->get_height();

	m_viewport.minDepth = 0.0f;
	m_viewport.maxDepth = 1.0f;

	m_scissor.offset = { 0, 0 };
	m_scissor.extent = { m_current_render_pass_builder->get_width(), m_current_render_pass_builder->get_height() };

	return backbuffer;
}

void VulkanBackend::reset_descriptor_builder()
{
	m_descriptor_builder_dirty = true;

	m_descriptor_builder.reset(
		&m_descriptor_pool_mgr,
		&m_descriptor_cache
	);
}

VulkanDescriptorBuilder VulkanBackend::get_descriptor_builder()
{
	if (!m_descriptor_builder_dirty) {
		return m_descriptor_builder;
	}

	u32 n_ubo_descriptors = (u32)m_ubo_mgr.get_descriptor_count();

	int n_tex = 0;
	for (; n_tex < m_image_infos.size(); n_tex++) {
		if (!m_image_infos[n_tex].imageView) {
			break;
		}
	}

	reset_descriptor_builder();

	// bind ubos
	for (int j = 0; j < n_ubo_descriptors; j++) {
		m_descriptor_builder.bind_buffer(
			j,
			&m_ubo_mgr.get_descriptor(j),
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
			VK_SHADER_STAGE_ALL_GRAPHICS
		);
	}

	// bind samplers
	for (int i = 0; i < n_tex; i++) {
		m_descriptor_builder.bind_image(
			n_ubo_descriptors + i,
			&m_image_infos[i],
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			VK_SHADER_STAGE_ALL_GRAPHICS
		);
	}

	m_descriptor_builder_dirty = false;
	return m_descriptor_builder;
}

VkDescriptorSet VulkanBackend::get_descriptor_set()
{
	u64 descriptor_set_hash = 0;

	for (int i = 0; i < m_image_infos.size(); i++) {
		if (!m_image_infos[i].imageView) {
			break;
		} else {
			hash::combine(&descriptor_set_hash, &m_image_infos[i]);
		}
	}

	VkDescriptorSet descriptor_set = {};
	VkDescriptorSetLayout descriptor_set_layout = {};

	VulkanDescriptorBuilder builder = get_descriptor_builder();
	builder.build(descriptor_set, descriptor_set_layout, descriptor_set_hash);

	return descriptor_set;
}

void VulkanBackend::clear_pipeline_cache()
{
	for (auto& [id, cache] : m_pipeline_cache) {
		vkDestroyPipeline(this->device, cache, nullptr);
	}

	m_pipeline_cache.clear();

	for (auto& [id, cache] : m_pipeline_layout_cache) {
		vkDestroyPipelineLayout(this->device, cache, nullptr);
	}

	m_pipeline_layout_cache.clear();
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
	} else if (counts & VK_SAMPLE_COUNT_1_BIT) {
		return VK_SAMPLE_COUNT_1_BIT;
	}

	wvn_ERROR("[VULKAN|DEBUG] Could not find a maximum usable sample count!");
	return VK_SAMPLE_COUNT_1_BIT;
}

void VulkanBackend::on_window_resize(int width, int height)
{
	m_backbuffer->on_window_resize(width, height);
}

void VulkanBackend::set_sample_shading(bool enabled, float min_sample_shading)
{
	m_sample_shading_enabled = enabled;
	m_min_sample_shading = min_sample_shading;
}

void VulkanBackend::set_cull_mode(CullMode cull)
{
	m_cull_mode = vkutil::get_vk_cull_mode(cull);
}

void VulkanBackend::set_texture(u32 idx, const Texture* texture)
{
	if (!texture) {
		m_image_infos[idx].imageView = VK_NULL_HANDLE;
		return;
	}

	VkImageView vk_image_view = ((const VulkanTexture*)texture)->image_view();
	m_image_infos[idx].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	if (m_image_infos[idx].imageView != vk_image_view)
	{
		m_image_infos[idx].imageView = vk_image_view;
		m_descriptor_builder_dirty = true;
	}
}

void VulkanBackend::set_sampler(u32 idx, TextureSampler* sampler)
{
	if (!sampler) {
		m_image_infos[idx].sampler = nullptr;
		return;
	}

	VkSampler vk_sampler = ((VulkanTextureSampler*)sampler)->bind(device, physical_data.properties, 4);

	if (m_image_infos[idx].sampler != vk_sampler)
	{
		m_image_infos[idx].sampler = vk_sampler;
		m_descriptor_builder_dirty = true;
	}
}

void VulkanBackend::bind_shader(const ShaderProgram* shader)
{
	if (!shader) {
		return;
	}

//	m_current_shader_parameters.clear();

	const VulkanShader* vksh = (const VulkanShader*)shader;
	m_shader_stages[vksh->type] = vksh->get_shader_stage_create_info();
}

void VulkanBackend::bind_shader_params(ShaderProgramType shader_type, ShaderParameters& params)
{
	ShaderParameters::PackedConstants packed_constants = params.get_packed_constants();

	if (packed_constants.size() <= 0) {
		return;
	}

//	m_current_shader_parameters[shader_type] = &params;

	bool modified = false;
	m_ubo_mgr.push_data(shader_type, packed_constants.data(), packed_constants.size(), &modified);

	if (modified) {
		m_descriptor_builder_dirty = true;
	}
}

void VulkanBackend::set_push_constants(ShaderParameters& params)
{
	m_push_constants = params.get_packed_constants();
}

void VulkanBackend::reset_push_constants()
{
	m_push_constants.clear();
}

void VulkanBackend::clear_descriptor_set_and_pool()
{
	m_descriptor_cache.clear_set_cache();
//	m_descriptor_pool_mgr.reset_pools();
}

void VulkanBackend::begin_render()
{
	vkWaitForFences(this->device, 1, &current_frame().in_flight_fence, VK_TRUE, UINT64_MAX);

	vkResetCommandPool(this->device, current_frame().command_pool, 0);
	VkCommandBuffer current_buffer = current_frame().command_buffer;

	VkCommandBufferBeginInfo command_buffer_begin_info = {};
	command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (VkResult result = vkBeginCommandBuffer(current_buffer, &command_buffer_begin_info); result != VK_SUCCESS) {
		wvn_ERROR("[VULKAN|DEBUG] Failed to begin recording command buffer: %d", result);
	}

	VkRenderPassBeginInfo render_pass_begin_info = m_current_render_pass_builder->build_begin_info();

	vkCmdBeginRenderPass(current_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanBackend::end_render()
{
	VkCommandBuffer current_buffer = current_frame().command_buffer;

	vkCmdEndRenderPass(current_buffer);

	if (VkResult result = vkEndCommandBuffer(current_buffer); result != VK_SUCCESS) {
		wvn_ERROR("[VULKAN|DEBUG] Failed to record command buffer: %d", result);
	}

	const VkSemaphore* wait_for_finish_semaphore = m_current_render_target->type() == RENDER_TARGET_TYPE_BACKBUFFER ? &m_backbuffer->get_image_available_semaphore() : nullptr;
	const VkSemaphore* queue_finished_semaphore = m_current_render_target->type() == RENDER_TARGET_TYPE_BACKBUFFER ? &m_backbuffer->get_render_finished_semaphore() : nullptr;
	int semaphore_count = m_current_render_target->type() == RENDER_TARGET_TYPE_BACKBUFFER ? 1 : 0;

	VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = semaphore_count;
	submit_info.pSignalSemaphores = queue_finished_semaphore;
	submit_info.signalSemaphoreCount = semaphore_count;
	submit_info.pWaitSemaphores = wait_for_finish_semaphore;
	submit_info.pWaitDstStageMask = &wait_stage;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &current_buffer;

	vkResetFences(this->device, 1, &current_frame().in_flight_fence);

	if (VkResult result = vkQueueSubmit(queues.graphics, 1, &submit_info, current_frame().in_flight_fence); result != VK_SUCCESS) {
		wvn_ERROR("[VULKAN|DEBUG] Failed to submit draw command to buffer: %d", result);
	}
}

void VulkanBackend::swap_buffers()
{
	vkWaitForFences(this->device, 1, &current_frame().in_flight_fence, VK_TRUE, UINT64_MAX);

	reset_descriptor_builder(); // todo: this should NOT be called every frame. figure out when to actually call this!

	m_backbuffer->swap_buffers();

	m_current_frame_idx = (m_current_frame_idx + 1) % vkutil::FRAMES_IN_FLIGHT;

	m_ubo_mgr.reset_ubo_usage_in_frame();

	m_backbuffer->acquire_next_image();
}

void VulkanBackend::render(const RenderOp& op)
{
	VkCommandBuffer current_buffer = current_frame().command_buffer;


	VkViewport viewport = {};

	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_current_render_pass_builder->get_width();
	viewport.height = (float)m_current_render_pass_builder->get_height();

	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};

	scissor.offset = { 0, 0 };
	scissor.extent = { m_current_render_pass_builder->get_width(), m_current_render_pass_builder->get_height() };

	vkCmdSetViewport(current_buffer, 0, 1, &viewport);
	vkCmdSetScissor(current_buffer, 0, 1, &scissor);

//	vkCmdSetViewport(current_buffer, 0, 1, &m_viewport);
//	vkCmdSetScissor(current_buffer, 0, 1, &m_scissor);

	const auto& vertex_buffer = ((VulkanBuffer*)op.vertex_data.buffer)->buffer();
	const auto& index_buffer  = ((VulkanBuffer*)op.index_data.buffer)->buffer();

	VkBuffer vertex_buffers[] = { vertex_buffer };
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindVertexBuffers(current_buffer, 0, 1, vertex_buffers, offsets);
	vkCmdBindIndexBuffer(current_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT16);

	VkPipeline pipeline = get_graphics_pipeline();
	VkPipelineLayout pipeline_layout = get_graphics_pipeline_layout();
	VkDescriptorSet descriptor_set = get_descriptor_set();

	if (m_push_constants.size() > 0)
	{
		vkCmdPushConstants(
			current_buffer,
			pipeline_layout,
			VK_SHADER_STAGE_ALL_GRAPHICS,
			0,
			m_push_constants.size(),
			m_push_constants.data()
		);
	}

	const auto& dynamic_offsets = m_ubo_mgr.get_dynamic_offsets();

	vkCmdBindDescriptorSets(
		current_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline_layout,
		0,
		1, &descriptor_set,
		dynamic_offsets.size(),
		dynamic_offsets.data()
	);

	vkCmdBindPipeline(
		current_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline
	);

	vkCmdDrawIndexed(
		current_buffer,
		op.index_data.indices->size(),
		1,
		0,
		0,
		0
	);
}

VulkanBackend::FrameData& VulkanBackend::current_frame()
{
	return frames[m_current_frame_idx];
}

u64 VulkanBackend::get_current_frame_idx() const
{
	return m_current_frame_idx;
}

void VulkanBackend::sync_stall() const
{
	while (Root::get_singleton()->system_backend()->get_window_size() == Vec2I::zero()) { }
	vkDeviceWaitIdle(this->device);
}

void VulkanBackend::set_viewport(const RectF &rect)
{
	m_viewport = { rect.x, rect.y, rect.w, rect.h };
}

void VulkanBackend::set_scissor(const RectI& rect)
{
	m_scissor = { rect.x, rect.y, (u32)rect.w, (u32)rect.h };
}

void VulkanBackend::set_render_target(RenderTarget* target)
{
	m_current_render_target = target;

	if (target->type() == RENDER_TARGET_TYPE_TEXTURE) {
		m_current_render_pass_builder = ((VulkanRenderTarget*)m_current_render_target)->get_render_pass_builder();
	} else if (target->type() == RENDER_TARGET_TYPE_BACKBUFFER) {
		m_current_render_pass_builder = ((VulkanBackbuffer*)m_current_render_target)->get_render_pass_builder();
	}
}

void VulkanBackend::set_depth_params(bool depth_test, bool depth_write)
{
	m_depth_stencil_create_info.depthTestEnable  = depth_test  ? VK_TRUE : VK_FALSE;
	m_depth_stencil_create_info.depthWriteEnable = depth_write ? VK_TRUE : VK_FALSE;
}

void VulkanBackend::set_depth_op(CompareOp op)
{
	m_depth_stencil_create_info.depthCompareOp = vkutil::get_vk_compare_op(op);
}

void VulkanBackend::set_depth_bounds_test(bool enabled)
{
	m_depth_stencil_create_info.depthTestEnable = enabled ? VK_TRUE : VK_FALSE;
}

void VulkanBackend::set_depth_bounds(float min, float max)
{
	m_depth_stencil_create_info.minDepthBounds = min;
	m_depth_stencil_create_info.maxDepthBounds = max;
}

void VulkanBackend::set_depth_stencil_test(bool enabled)
{
	m_depth_stencil_create_info.stencilTestEnable = enabled ? VK_TRUE : VK_FALSE;
}

void VulkanBackend::toggle_blending(bool enabled)
{
	m_colour_blend_attachment_state.blendEnable = enabled ? VK_TRUE : VK_FALSE;
}

void VulkanBackend::set_blend_write_mask(bool r, bool g, bool b, bool a)
{
	m_colour_blend_attachment_state.colorWriteMask = 0;

	if (r) m_colour_blend_attachment_state.colorWriteMask |= VK_COLOR_COMPONENT_R_BIT;
	if (g) m_colour_blend_attachment_state.colorWriteMask |= VK_COLOR_COMPONENT_G_BIT;
	if (b) m_colour_blend_attachment_state.colorWriteMask |= VK_COLOR_COMPONENT_B_BIT;
	if (a) m_colour_blend_attachment_state.colorWriteMask |= VK_COLOR_COMPONENT_A_BIT;
}

void VulkanBackend::set_blend_colour(const Blend& blend)
{
	m_colour_blend_attachment_state.colorBlendOp = vkutil::get_vk_blend_op(blend.op);
	m_colour_blend_attachment_state.srcColorBlendFactor = vkutil::get_vk_blend_factor(blend.src);
	m_colour_blend_attachment_state.dstColorBlendFactor = vkutil::get_vk_blend_factor(blend.dst);
}

void VulkanBackend::set_blend_alpha(const Blend& blend)
{
	m_colour_blend_attachment_state.alphaBlendOp = vkutil::get_vk_blend_op(blend.op);
	m_colour_blend_attachment_state.srcAlphaBlendFactor = vkutil::get_vk_blend_factor(blend.src);
	m_colour_blend_attachment_state.dstAlphaBlendFactor = vkutil::get_vk_blend_factor(blend.dst);
}

void VulkanBackend::set_blend_constants(float r, float g, float b, float a)
{
	m_blend_constants[0] = r;
	m_blend_constants[1] = g;
	m_blend_constants[2] = b;
	m_blend_constants[3] = a;
}

void VulkanBackend::get_blend_constants(float* constants)
{
	mem::copy(constants, m_blend_constants, sizeof(float) * 4);
}

void VulkanBackend::set_blend_op(bool enabled, LogicOp op)
{
	m_blend_state_logic_op_enabled = enabled;
	m_blend_state_logic_op = vkutil::get_vk_logic_op(op);
	// todo: VkStencilOp ????
}

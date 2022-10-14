
// https://vulkan-tutorial.com
// Thank. You. So. Much.

#include <backend/graphics/vulkan/vulkan_backend.h>

#include <wvn/root.h>
#include <wvn/system/system_backend.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

static const char* VALIDATION_LAYERS[] = {
	"VK_LAYER_KHRONOS_validation"
};

static const char* DEVICE_EXTENSIONS[] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
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

static Vector<const char*> get_device_extensions()
{
	Vector<const char*> result;

	result.push_back("VK_KHR_portability_subset");

	return result;
}

VulkanBackend::VulkanBackend()
	: m_instance(VK_NULL_HANDLE)
	, m_surface(VK_NULL_HANDLE)
	, m_queues()
	, m_logical_data()
	, m_physical_data()
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

	if (VkResult result = vkCreateInstance(&create_info, nullptr, &m_instance); result != VK_SUCCESS)
	{
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
		WVN_ERROR("[VULKAN] Failed to create surface.");
	}

	enumerate_physical_devices();
	create_logical_device();

	dev::LogMgr::get_singleton().print("[VULKAN] Initialized!");
}

VulkanBackend::~VulkanBackend()
{
#if WVN_DEBUG
	if (g_debug_enable_validation_layers) {
		debug_destroy_debug_utils_messenger_ext(m_instance, m_debug_messenger, nullptr);
	}
#endif

	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkDestroyDevice(m_logical_data.device, nullptr);
	vkDestroyInstance(m_instance, nullptr);

	dev::LogMgr::get_singleton().print("[VULKAN] Destroyed!");
}

RendererProperties VulkanBackend::properties()
{
	RendererProperties properties = {};

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

	u32 usability0 = assign_physical_device_usability(m_physical_data.device, properties, features);

	// select the device of the highest usability
	for (int i = 1; i < device_count; i++)
	{
		vkGetPhysicalDeviceProperties(devices[i], &m_physical_data.properties);
		vkGetPhysicalDeviceFeatures(devices[i], &m_physical_data.features);

		u32 usability1 = assign_physical_device_usability(devices[i], properties, features);

		if (usability1 > usability0)
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

	dev::LogMgr::get_singleton().print("[VULKAN] Selected suitable GPU: %d", m_physical_data.device);
}

void VulkanBackend::create_logical_device()
{
	constexpr float QUEUE_PRIORITY = 1.0f;

	QueueFamilyIdx idx = find_queue_families(m_physical_data.device);

	Vector<VkDeviceQueueCreateInfo> queue_create_infos;

	for (auto family : idx.package())
	{
		queue_create_infos.push_back({
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = family,
			.queueCount = 1,
			.pQueuePriorities = &QUEUE_PRIORITY
		});
	}

	auto extensions = get_device_extensions();

	VkDeviceCreateInfo device_create_info = {};
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.queueCreateInfoCount = static_cast<u32>(queue_create_infos.size());
	device_create_info.pQueueCreateInfos = queue_create_infos.data();
	device_create_info.enabledLayerCount = 0;
	device_create_info.ppEnabledLayerNames = nullptr;
	device_create_info.ppEnabledExtensionNames = extensions.data();
	device_create_info.enabledExtensionCount = static_cast<u32>(extensions.size());
	device_create_info.pEnabledFeatures = &m_physical_data.features;

#if WVN_DEBUG
	if (g_debug_enable_validation_layers) {
		device_create_info.enabledLayerCount = ARRAY_LENGTH(VALIDATION_LAYERS);
		device_create_info.ppEnabledLayerNames = VALIDATION_LAYERS;
	}
#endif

	if (VkResult result = vkCreateDevice(m_physical_data.device, &device_create_info, nullptr, &m_logical_data.device); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:DEBUG] Failed to create logical device.");
	}

	vkGetDeviceQueue(m_logical_data.device, idx.graphics_family.value(), 0, &m_queues.graphics_queue);
	vkGetDeviceQueue(m_logical_data.device, idx.present_family.value(),  0, &m_queues.present_queue);
	vkGetDeviceQueue(m_logical_data.device, idx.compute_family.value(),  0, &m_queues.compute_queue);
	vkGetDeviceQueue(m_logical_data.device, idx.transfer_family.value(), 0, &m_queues.transfer_queue);

	dev::LogMgr::get_singleton().print("[VULKAN] Created a logical device!");
}

// abstract function that generates a "usability" or "goodness value" of a given device
u32 VulkanBackend::assign_physical_device_usability(
	VkPhysicalDevice device,
	VkPhysicalDeviceProperties properties,
	VkPhysicalDeviceFeatures features
)
{
	u32 result = 0;

	bool is_correct_device = false;//properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU; // note, isnt my M1 chip integrated graphics! this may cause issues so look here first!!!!!!!!!!!
	bool has_required_features = false;//features.geometryShader;
	bool has_required_extensions = check_device_extension_support(device);
	bool indices_complete = find_queue_families(device).is_complete();

	if (is_correct_device) {
		result += 1;
	}

	if (has_required_features) {
		result += 1;
	}

	if (has_required_extensions) {
		result += 1;
	}

	if (indices_complete) {
		result += 1;
	}

	return result;
}

VulkanBackend::QueueFamilyIdx VulkanBackend::find_queue_families(VkPhysicalDevice device)
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

	auto device_exts = get_device_extensions();

	Vector<VkExtensionProperties> available_exts(ext_count);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &ext_count, available_exts.data());

	Vector<const char*> required_exts(device_exts.begin(), device_exts.end());

	for (const auto& available_extension : available_exts) {
		for (int i = 0; i < required_exts.size(); i++) {
			if (available_extension.extensionName == required_exts[i]) {
				required_exts.erase(i);
			}
		}
	}

	return required_exts.empty();
}

// temporary function while i learn how tf vulkan actually works lol
void VulkanBackend::debug_tick()
{
}

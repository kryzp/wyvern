
// https://vulkan-tutorial.com
// Thank. You. So. Much.

#if 0

VkQueue graphics_queue;

vkGetDeviceQueue(m_device, idx.graphics_family.value(), 0, &graphics_queue);

#endif

#include <backend/graphics/vulkan/vulkan_backend.h>

#include <wvn/root.h>
#include <wvn/system/system_backend.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/container/optional.h>

using namespace wvn;
using namespace wvn::gfx;

static const char* VALIDATION_LAYERS[] = {
	"VK_LAYER_KHRONOS_validation"
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

struct QueueFamilyIdx
{
	Optional<u32> graphics_family;
	Optional<u32> present_family;

	constexpr bool is_complete() const
	{
		return (
			graphics_family.has_value() &&
			present_family.has_value()
		);
	}
};

static QueueFamilyIdx find_queue_families(VkPhysicalDevice device)
{
	QueueFamilyIdx result;

	u32 queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

	if (!queue_family_count) {
		WVN_ERROR("[VULKAN] Failed to find any queue families!");
	}

	Vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

	for (int i = 0; i < queue_family_count; i++) {
		if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			result.graphics_family = i;
		}
	}

	return result;
}

VulkanBackend::VulkanBackend()
	: m_instance()
	, m_logical_data()
	, m_physical_data()
#if WVN_DEBUG
	, m_debug_messenger()
#endif
{
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = Root::get_singleton().config().name;
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "Wyvern";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

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
	// this may go out of scope (?) check if this causes errors !
	constexpr float QUEUE_PRIORITY = 1.0f;

	QueueFamilyIdx idx = find_queue_families(m_physical_data.device);

	VkDeviceQueueCreateInfo queue_create_info = {};
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueFamilyIndex = idx.graphics_family.value_or(0);
	queue_create_info.queueCount = 1;
	queue_create_info.pQueuePriorities = &QUEUE_PRIORITY;

	VkDeviceCreateInfo device_create_info = {};
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pQueueCreateInfos = &queue_create_info;
	device_create_info.queueCreateInfoCount = 1;
	device_create_info.pEnabledFeatures = &m_physical_data.features;

	device_create_info.enabledLayerCount = 0;
	device_create_info.ppEnabledLayerNames = nullptr;

	auto extensions = get_device_extensions();

	device_create_info.enabledExtensionCount = extensions.size();
	device_create_info.ppEnabledExtensionNames = extensions.data();

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

	if (is_correct_device) {
		result += 1 << 0;
	}

	if (has_required_features) {
		result += 1 << 1;
	}

	QueueFamilyIdx idx = find_queue_families(device);

	if (idx.is_complete()) {
		result += 1 << 2;
	}

	return result;
}

// temporary function while i learn how tf vulkan actually works lol
void VulkanBackend::debug_tick()
{
}

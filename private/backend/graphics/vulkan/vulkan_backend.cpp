#include <backend/graphics/vulkan/vulkan_backend.h>

#include <wvn/root.h>
#include <wvn/system/system_backend.h>
#include <wvn/container/vector.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

#if 0
std::vector<const char*> requiredExtensions;

for(uint32_t i = 0; i < glfwExtensionCount; i++) {
    requiredExtensions.emplace_back(glfwExtensions[i]);
}

requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)

createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

createInfo.enabledExtensionCount = (uint32_t) requiredExtensions.size();
createInfo.ppEnabledExtensionNames = requiredExtensions.data();

if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
}
#endif

VulkanBackend::VulkanBackend()
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

	// extensions
	u32 ext_count = 0;

	if (!Root::get_singleton().current_system_backend()->vk_get_instance_extensions(&ext_count, nullptr)) {
		WVN_ERROR("[VULKAN]: Unable to get instance extension count.");
	}

	Vector<const char*> extensions(ext_count);

	if (!Root::get_singleton().current_system_backend()->vk_get_instance_extensions(&ext_count, extensions.data())) {
		WVN_ERROR("[VULKAN]: Unable to get additional instance extensions.");
	}

	//extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

	create_info.enabledExtensionCount = ext_count + 1;
	create_info.ppEnabledExtensionNames = extensions.data();
	create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

	if (VkResult result = vkCreateInstance(&create_info, nullptr, &m_instance); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton().print("[VULKAN]: %d", result);
		WVN_ERROR("[VULKAN] Failed to create instance.");
	}

	//uint32_t ext_count1 = 0;
	//vkEnumerateInstanceExtensionProperties(nullptr, &ext_count1, nullptr);

	//Vector<VkExtensionProperties> extensions(ext_count1);
	//vkEnumerateInstanceExtensionProperties(nullptr, &ext_count1, extensions.data());

	//LogMgr::get_singleton().print("Available Extensions: ");

	//for (const auto& extension : extensions) {
	//	LogMgr::get_singleton().print(extension.extensionName");
	//}
}

VulkanBackend::~VulkanBackend()
{
	vkDestroyInstance(m_instance, nullptr);
}

RendererProperties VulkanBackend::properties()
{
	RendererProperties properties = {};
	properties.origin_bottom_left = true;

	return properties;
}

// temporary function while i learn how tf vulkan actually works lol
void VulkanBackend::tick()
{
}

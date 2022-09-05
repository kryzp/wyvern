#include <wvn/plugin/graphics/vulkan_plugin.h>
#include <backend/graphics/vulkan_backend.h>
#include <wvn/root.h>

using namespace wvn;

void VulkanPlugin::install()
{
	m_backend = new bknd::VulkanBackend();
	Root::get_singleton().set_rendering_backend((bknd::VulkanBackend*)m_backend);
}

void VulkanPlugin::uninstall()
{
	delete (bknd::VulkanBackend*)m_backend;
}

#include <wvn/plugin/graphics/vulkan_plugin.h>
#include <backend/graphics/vulkan_backend.h>
#include <wvn/root.h>

using namespace wvn;
using namespace wvn::plug;

void VulkanPlugin::install()
{
	m_backend = new gfx::VulkanBackend();
	Root::get_singleton().set_rendering_backend((gfx::VulkanBackend*)m_backend);
}

void VulkanPlugin::uninstall()
{
	delete (gfx::VulkanBackend*)m_backend;
}

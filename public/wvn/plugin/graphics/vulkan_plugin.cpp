#include <wvn/plugin/graphics/vulkan_plugin.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <wvn/root.h>

using namespace wvn;
using namespace wvn::plug;

void VulkanPlugin::implement()
{
	m_backend = new gfx::VulkanBackend();
	Root::get_singleton()->set_rendering_backend(static_cast<gfx::VulkanBackend*>(m_backend));
}

void VulkanPlugin::remove()
{
	delete static_cast<gfx::VulkanBackend*>(m_backend);
}

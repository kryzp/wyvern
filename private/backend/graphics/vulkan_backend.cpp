#include <backend/graphics/vulkan_backend.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanBackend::VulkanBackend()
{
}

VulkanBackend::~VulkanBackend()
{
}

RendererProperties VulkanBackend::properties()
{
	RendererProperties properties = {};
	properties.origin_bottom_left = true;

	return properties;
}

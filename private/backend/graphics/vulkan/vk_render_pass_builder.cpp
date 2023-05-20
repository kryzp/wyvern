#include <backend/graphics/vulkan/vk_render_pass_builder.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanRenderPassBuilder::VulkanRenderPassBuilder()
	: m_clear_colours()
{
}

VulkanRenderPassBuilder::~VulkanRenderPassBuilder()
{
}

VkRenderPassBeginInfo VulkanRenderPassBuilder::begin_info(VkRenderPass render_pass, VkFramebuffer framebuffer, VkExtent2D extent) const
{
	VkRenderPassBeginInfo render_pass_begin_info = {};

	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass = render_pass;
	render_pass_begin_info.framebuffer = framebuffer;
	render_pass_begin_info.renderArea.offset = { 0, 0 };
	render_pass_begin_info.renderArea.extent = extent;
	render_pass_begin_info.pClearValues = m_clear_colours.data();
	render_pass_begin_info.clearValueCount = m_clear_colours.size();

	return render_pass_begin_info;
}

void VulkanRenderPassBuilder::set_clear_colour(const Colour& colour)
{
	colour.premultiplied().export_to_float(m_clear_colours[0].color.float32);
}

void VulkanRenderPassBuilder::set_depth_stencil_clear(float depth, u32 stencil)
{
	m_clear_colours[1].depthStencil = { depth, stencil };
}

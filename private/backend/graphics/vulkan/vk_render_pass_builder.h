#ifndef VK_RENDER_PASS_BUILDER_H
#define VK_RENDER_PASS_BUILDER_H

#include <wvn/maths/colour.h>
#include <wvn/container/array.h>
#include <vulkan/vulkan.h>

namespace wvn::gfx
{
	class VulkanBackend;

	// this name kind of sucks
	class VulkanRenderPassBuilder
	{
	public:
		VulkanRenderPassBuilder();
		~VulkanRenderPassBuilder();

		VkRenderPassBeginInfo begin_info(VkRenderPass render_pass, VkFramebuffer framebuffer, VkExtent2D extent) const;

		void set_clear_colour(const Colour& colour);
		void set_depth_stencil_clear(float depth, u32 stencil);

	private:
		Array<VkClearValue, 2> m_clear_colours;
	};
}

#endif // VK_RENDER_PASS_BUILDER_H

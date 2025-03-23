#ifndef VK_RENDER_PASS_BUILDER_H_
#define VK_RENDER_PASS_BUILDER_H_

#include <wvn/maths/colour.h>
#include <wvn/container/array.h>
#include <wvn/container/vector.h>
#include <vulkan/vulkan.h>
#include <wvn/graphics/render_target.h>

namespace wvn::gfx
{
	class VulkanBackend;
	class VulkanBackbuffer;

	using ClearValues = Vector<VkClearValue>;

	class VulkanRenderPassBuilder
	{
	public:
		VulkanRenderPassBuilder();
		~VulkanRenderPassBuilder();

		void init(VulkanBackend* backend);

		void clean_up();
		void clean_up_fbos();

		void create_render_pass(u64 n_framebuffers, u64 attachment_count, VkImageView* attachments, VkImageView* backbuffer_attachments, u64 backbuffer_attachment_idx);

		void create_colour_attachment(int idx, VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp load_op, VkImageLayout final_layout, bool is_resolve);
		void create_depth_attachment(int idx, VkSampleCountFlagBits samples);

		VkRenderPassBeginInfo build_begin_info() const;

		void make_backbuffer(VulkanBackbuffer* backbuffer);

		void set_clear_colour(int idx, VkClearValue value);
		void set_clear_colours(const ClearValues& values);

		VkRenderPass get_render_pass();

		void set_dimensions(u32 width, u32 height);

		int get_colour_attachment_count() const;
		int get_attachment_count() const;

		u32 get_width() const;
		u32 get_height() const;

	private:
		void destroy_render_pass();
		void destroy_fbos();

		VulkanBackend* m_backend;

		VulkanBackbuffer* m_backbuffer;

		u32 m_width;
		u32 m_height;

		ClearValues m_clear_colours;
		VkRenderPass m_render_pass;

		Array<VkAttachmentDescription, wvn_MAX_RENDER_TARGET_ATTACHMENTS> m_attachment_descriptions;

		Vector<VkAttachmentReference> m_colour_attachments;
		Vector<VkAttachmentReference> m_colour_attachment_resolves;
		VkAttachmentReference m_depth_attachment_ref;

		Vector<VkFramebuffer> m_framebuffers;

		int m_colour_attachment_count;
		int m_attachment_count;
	};
}

#endif // VK_RENDER_PASS_BUILDER_H_

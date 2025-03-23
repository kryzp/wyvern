#ifndef VK_RENDER_TARGET_H_
#define VK_RENDER_TARGET_H_

#include <wvn/graphics/render_target.h>
#include <vulkan/vulkan.h>
#include <wvn/common.h>
#include <wvn/container/array.h>
#include <backend/graphics/vulkan/vk_render_pass_builder.h>
#include <backend/graphics/vulkan/vk_texture.h>

namespace wvn::gfx
{
	class VulkanBackend;
	class VulkanRenderPassBuilder;

	class VulkanRenderTarget : public RenderTarget
	{
	public:
		VulkanRenderTarget(VulkanBackend* backend);
		VulkanRenderTarget(VulkanBackend* backend, u32 width, u32 height);
		~VulkanRenderTarget() override;

		void create();
		void create_only_depth();

		void clean_up() override;

		VulkanRenderPassBuilder* get_render_pass_builder();

		void set_clear_colour(const Colour& colour) override;
		void set_depth_stencil_clear(float depth, u32 stencil) override;

		const Texture* get_attachment(int idx) const override;
		const Texture* get_depth_attachment() const override;

		void set_attachment(int idx, VulkanTexture* tex);

		int get_msaa() const override;

	private:
		void create_depth_resources(int idx);

		VulkanBackend* m_backend;
		VulkanRenderPassBuilder m_render_pass_builder;

		Array<VulkanTexture*, wvn_MAX_RENDER_TARGET_ATTACHMENTS> m_attachments;
		VulkanTexture m_depth;
	};
}

#endif // VK_RENDER_TARGET_H_

#ifndef VK_BACKBUFFER_H_
#define VK_BACKBUFFER_H_

#include <backend/graphics/vulkan/vk_texture.h>
#include <backend/graphics/vulkan/vk_util.h>
#include <backend/graphics/vulkan/vk_render_pass_builder.h>
#include <wvn/graphics/backbuffer.h>

namespace wvn::gfx
{
	class VulkanBackend;

	/**
	 * Special case of the render target for the regular window since it gets
	 * treated slightly differently, especially since it contains multiple swaps.
	 */
	class VulkanBackbuffer : public Backbuffer
	{
	public:
		VulkanBackbuffer(VulkanBackend* backend);
		~VulkanBackbuffer() override;

		void create();
		void create_surface();

		void clean_up() override;
        void clean_up_swap_chain();
		void clean_up_textures();

		VulkanRenderPassBuilder* get_render_pass_builder();

		void set_clear_colour(const Colour& colour) override;
		void set_depth_stencil_clear(float depth, u32 stencil) override;

        void acquire_next_image();
        void swap_buffers();

		const Texture* get_attachment(int idx) const override;
		const Texture* get_depth_attachment() const override;

        void create_swap_chain();
        void create_swap_chain_image_views();

		void on_window_resize(int width, int height);

		VkSurfaceKHR get_surface() const;

		int get_msaa() const override;

		int get_current_texture_idx() const;

		const VkSemaphore& get_render_finished_semaphore() const;
		const VkSemaphore& get_image_available_semaphore() const;

	private:
		void create_colour_resources();
        void create_depth_resources();
		void create_swap_chain_sync_objects();

		void rebuild_swap_chain();

		u32 m_width;
		u32 m_height;

		VulkanBackend* m_backend;
		VulkanRenderPassBuilder m_render_pass_builder;

		Array<VkSemaphore, vkutil::FRAMES_IN_FLIGHT> m_render_finished_semaphores;
		Array<VkSemaphore, vkutil::FRAMES_IN_FLIGHT> m_image_available_semaphores;

        VkSwapchainKHR m_swap_chain;
        Vector<VkImage> m_swap_chain_images;
        Vector<VkImageView> m_swap_chain_image_views;
        VkSurfaceKHR m_surface;
        u32 m_curr_swap_chain_image_idx;

		VulkanTexture m_colour;
        VulkanTexture m_depth;
	};
}

#endif // VK_BACKBUFFER_H_

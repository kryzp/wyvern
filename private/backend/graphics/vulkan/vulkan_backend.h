#pragma once

#include <vulkan/vulkan.h>

#include <wvn/container/vector.h>
#include <wvn/container/optional.h>

#include <wvn/util/types.h>
#include <wvn/graphics/renderer_backend.h>

namespace wvn::gfx
{
	struct QueueFamilyIdx
	{
		Optional<u32> graphics_family;
		Optional<u32> present_family;
//		Optional<u32> compute_family;
//		Optional<u32> transfer_family;

		constexpr bool is_complete() const
		{
			return (
				graphics_family &&
				present_family// &&
//				compute_family &&
//				transfer_family
			);
		}

		constexpr bool all_unique() const
		{
			// todo: store/cache this??
			const u32 g = graphics_family.value();
			const u32 p = present_family.value();
//			const u32 c = compute_family.value();
//			const u32 t = transfer_family.value();

			return (
				(g != p)
			);

//			return (
//				(g != p && g != c && g != t) &&
//				(p != g && p != c && p != t) &&
//				(c != g && c != p && c != t) &&
//				(t != g && t != c && t != p)
//			);
		}

		const Vector<u32> package() const
		{
			return {
				graphics_family.value(),
				present_family.value()//,
//				compute_family.value(),
//				transfer_family.value()
			};
		}
	};

	struct QueueData
	{
		VkQueue graphics_queue;
		VkQueue present_queue;
//		VkQueue compute_queue;
//		VkQueue transfer_queue;
	};

	struct LogicalDeviceData
	{
		VkDevice device;
	};

	struct PhysicalDeviceData
	{
		VkPhysicalDevice device;
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		Vector<VkSurfaceFormatKHR> surface_formats;
		Vector<VkPresentModeKHR> present_modes;
	};

	class VulkanBackend : public RendererBackend
	{
	public:
		VulkanBackend();
		~VulkanBackend() override;

		RendererProperties properties() override;

		// debug //
		void debug_render() override;
		// debug //

		void render(const RenderPass& pass) override;

		void wait_for_sync() override;
		void clear(const Colour& colour = Colour::empty()) override;

		Ref<Texture> create_texture(u32 width, u32 height) override;
		Ref<Shader> create_shader(const Vector<char>& vert_source, const Vector<char>& frag_source) override;
		Ref<RenderTarget> create_render_target(u32 width, u32 height) override;
		Ref<Mesh> create_mesh() override;

	private:
		void enumerate_physical_devices();
		void create_logical_device(const QueueFamilyIdx& phys_idx);
		void create_swap_chain(const QueueFamilyIdx& phys_idx);
		void create_graphics_pipeline();
		void create_render_pass();
		void create_command_pool(const QueueFamilyIdx& phys_idx);
		void create_command_buffers();
		void create_sync_objects();

		void create_image_views();
		void clean_up_swap_chain();
		void rebuild_swap_chain();
		void create_swap_chain_framebuffers();

		void record_command_buffer(VkCommandBuffer cmd_buf, u32 img_idx);
		u32 assign_physical_device_usability(VkPhysicalDevice device, VkPhysicalDeviceProperties properties, VkPhysicalDeviceFeatures features, bool* essentials_completed);
		QueueFamilyIdx find_queue_families(VkPhysicalDevice device);
		bool check_device_extension_support(VkPhysicalDevice device);
		SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);
		VkSurfaceFormatKHR choose_swap_surface_format(const Vector<VkSurfaceFormatKHR>& available_surface_formats);
		VkPresentModeKHR choose_swap_present_mode(const Vector<VkPresentModeKHR>& available_present_modes);
		VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);
		VkShaderModule create_shader_module(const Vector<char>& source);

		// core
		VkInstance m_instance;
		VkSurfaceKHR m_surface;
		u64 m_current_frame;

		// commands
		VkCommandPool m_command_pool;
		Vector<VkCommandBuffer> m_command_buffers;

		// sync
		Vector<VkSemaphore> m_image_available_semaphores;
		Vector<VkSemaphore> m_render_finished_semaphores;
		Vector<VkFence> m_in_flight_fences;

		// render pass
		VkRenderPass m_render_pass;
		VkPipelineLayout m_pipeline_layout;
		VkPipeline m_graphics_pipeline;

		// swap chain
		VkSwapchainKHR m_swap_chain;
		Vector<VkImage> m_swap_chain_images;
		Vector<VkImageView> m_swap_chain_image_views;
		VkFormat m_swap_chain_image_format;
		VkExtent2D m_swap_chain_extent;
		Vector<VkFramebuffer> m_swap_chain_framebuffers;

		// data
		QueueData m_queues;
		LogicalDeviceData m_logical_data;
		PhysicalDeviceData m_physical_data;

#if WVN_DEBUG
		VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
	};
}

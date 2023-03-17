#ifndef VK_BACKEND_H
#define VK_BACKEND_H

#include <vulkan/vulkan.h>

#include <wvn/container/vector.h>
#include <wvn/container/optional.h>

#include <wvn/root.h>
#include <wvn/util/types.h>
#include <wvn/graphics/renderer_backend.h>

#include <backend/graphics/vulkan/vk_buffer.h>
#include <backend/graphics/vulkan/vk_texture.h>

#include <wvn/maths/mat4x4.h>

namespace wvn::gfx
{
	struct UniformBufferObject
	{
		alignas(16) Mat4x4 model;
		alignas(16) Mat4x4 view;
		alignas(16) Mat4x4 proj;
	};

	struct QueueFamilyIdx
	{
		Optional<u32> graphics_family;
		Optional<u32> present_family;
		Optional<u32> compute_family;
		Optional<u32> transfer_family;

		constexpr bool is_complete() const
		{
			return (
				graphics_family &&
				present_family &&
				compute_family &&
				transfer_family
			);
		}

		constexpr bool all_unique() const
		{
			const u32 g = graphics_family.value();
			const u32 p = present_family.value();
			const u32 c = compute_family.value();
			const u32 t = transfer_family.value();

			return (
				(g != p) && (g != t) && (t != p) && (t != c) && (g != c) && (p != c)
			);
		}

		const Vector<u32> package() const
		{
			return {
				graphics_family.value(),
				present_family.value(),
				compute_family.value(),
				transfer_family.value()
			};
		}
	};

	struct QueueData
	{
		VkQueue graphics;
		VkQueue present;
		VkQueue compute;
		VkQueue transfer;
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

		RendererBackendProperties properties() override;

		void render(const RenderPass& pass) override;

		void wait_for_sync() override;

		void on_window_resize(int width, int height) override;

		const LogicalDeviceData& logical_data() const;
		const PhysicalDeviceData& physical_data() const;
		const QueueData& queues() const;

	private:
		void enumerate_physical_devices();
		void create_logical_device(const QueueFamilyIdx& phys_idx);
		void create_swap_chain(const QueueFamilyIdx& phys_idx);
		void create_graphics_pipeline();
		void create_render_pass();
		void create_command_pool(const QueueFamilyIdx& phys_idx);
		void create_command_buffers();
		void create_sync_objects();
		void create_vertex_buffer();
		void create_index_buffer();
		void create_descriptor_set_layout();
		void create_uniform_buffers();
        void create_descriptor_pool();
        void create_descriptor_sets();
		void create_texture_image();
		void create_depth_texture();
		void create_image_views();
		void clean_up_swap_chain();
		void rebuild_swap_chain();
		void create_swap_chain_framebuffers();

		u32 assign_physical_device_usability(VkPhysicalDevice device, VkPhysicalDeviceProperties properties, VkPhysicalDeviceFeatures features, bool* essentials_completed);
		QueueFamilyIdx find_queue_families(VkPhysicalDevice device);
		bool check_device_extension_support(VkPhysicalDevice device);

		SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);

		VkShaderModule create_shader_module(const Vector<char>& source) const;

		// core
		VkInstance m_instance;
		VkSurfaceKHR m_surface;
		u64 m_current_frame;

		// commands
		VkCommandPool m_command_pool;
		Vector<VkCommandBuffer> m_command_buffers;

		// vertex stuff
		VulkanBuffer m_staging_buffer;
		VulkanBuffer m_vertex_buffer;
		VulkanBuffer m_index_buffer;
		Vector<VulkanBuffer> m_uniform_buffers;

		// sync
		Vector<VkSemaphore> m_image_available_semaphores;
		Vector<VkSemaphore> m_render_finished_semaphores;
		Vector<VkFence> m_in_flight_fences;
		bool m_is_framebuffer_resized;

		// render pass
		VkRenderPass m_render_pass;
		VkPipeline m_graphics_pipeline;
		VkPipelineLayout m_pipeline_layout;
		VkDescriptorSetLayout m_descriptor_set_layout;
        VkDescriptorPool m_descriptor_pool;
        Vector<VkDescriptorSet> m_descriptor_sets;

		// swap chain
		VkSwapchainKHR m_swap_chain;
		Vector<VulkanImage> m_swap_chain_images;
		Vector<VulkanImageView> m_swap_chain_image_views;
		VkFormat m_swap_chain_image_format;
		VkExtent2D m_swap_chain_extent;
		Vector<VkFramebuffer> m_swap_chain_framebuffers;

		// data
		QueueData m_queues;
		LogicalDeviceData m_logical_data;
		PhysicalDeviceData m_physical_data;

		// depth
		VulkanTexture m_depth;

		// TEMP //
		VkShaderModule m_temp_vert_module;
		VkShaderModule m_temp_frag_module;
		VulkanTexture m_temp_texture;
		// TEMP //

#if WVN_DEBUG
		VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
	};
}

#endif // VK_BACKEND_H

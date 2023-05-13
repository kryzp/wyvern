#ifndef VK_BACKEND_H
#define VK_BACKEND_H

#include <vulkan/vulkan.h>

#include <wvn/container/vector.h>
#include <wvn/container/array.h>
#include <wvn/container/optional.h>
#include <wvn/container/hash_map.h>

#include <wvn/root.h>
#include <wvn/util/common.h>
#include <wvn/graphics/renderer_backend.h>

#include <backend/graphics/vulkan/vk_buffer.h>
#include <backend/graphics/vulkan/vk_texture.h>
#include <backend/graphics/vulkan/vk_shader.h>

#include <backend/graphics/vulkan/vk_buffer_mgr.h>
#include <backend/graphics/vulkan/vk_texture_mgr.h>
#include <backend/graphics/vulkan/vk_shader_mgr.h>

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
		struct FrameData
		{
			Ref<VulkanBuffer> uniform_buffer;
			VkFence in_flight_fence;
			VkSemaphore render_finished_semaphore;
			VkSemaphore image_available_semaphore;
			VkCommandPool command_pool;
			VkCommandBuffer command_buffer;
		};

	public:
		static constexpr u32 MAX_FRAMES_IN_FLIGHT = 2;

		VulkanBackend();
		~VulkanBackend() override;

		RendererBackendProperties properties() override;

		void render(const RenderPass& pass) override;
		void swap_buffers() override;
		void wait_for_sync();

		void on_window_resize(int width, int height) override;

		void set_texture(u32 idx, const Texture* texture) override;
		void set_sampler(u32 idx, TextureSampler* sampler) override;

		void bind_shader(Shader* shader) override;

		u64 frame() const;

		VkDevice device;
		PhysicalDeviceData physical_data;
		QueueData queues;
		FrameData frames[MAX_FRAMES_IN_FLIGHT];

	private:
		void enumerate_physical_devices();
		void create_logical_device(const QueueFamilyIdx& phys_idx);
		void create_swap_chain(const QueueFamilyIdx& phys_idx);
		void create_render_pass();
		void create_command_pools(const QueueFamilyIdx& phys_idx);
		void create_command_buffers();
		void create_sync_objects();
		void create_descriptor_set_layout();
		void create_uniform_buffers();
        void create_descriptor_pool();
        void create_descriptor_sets();
		void create_depth_texture();
		void create_image_views();
		void clean_up_swap_chain();
		void rebuild_swap_chain();
		void create_swap_chain_framebuffers();
		void acquire_next_image();
		void clear_pipeline_cache();

		VkPipeline get_graphics_pipeline();

		const Vector<VkDescriptorSet>& get_descriptor_sets();
		void update_descriptor_sets();

		u32 assign_physical_device_usability(VkPhysicalDevice physical_device, VkPhysicalDeviceProperties properties, VkPhysicalDeviceFeatures features, bool* essentials_completed);
		QueueFamilyIdx find_queue_families(VkPhysicalDevice physical_device);
		bool check_device_extension_support(VkPhysicalDevice physical_device);
		SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice physical_device);

		// core
		VkInstance m_instance;
		VkSurfaceKHR m_surface;
		u64 m_current_frame;
		bool m_is_framebuffer_resized;

		// managers
		VulkanBufferMgr* m_buffer_mgr;
		VulkanTextureMgr* m_texture_mgr;
		VulkanShaderMgr* m_shader_mgr;

		// render pass
		VkRenderPass m_render_pass;
		VkPipelineLayout m_pipeline_layout;
		HashMap<u32, VkPipeline> m_pipeline_cache;
		VkDescriptorSetLayout m_descriptor_set_layout;
        VkDescriptorPool m_descriptor_pool;
        Vector<VkDescriptorSet> m_descriptor_sets;
		Array<VkWriteDescriptorSet, 2> m_descriptor_writes;
		Array<VkDescriptorImageInfo, 1> m_image_infos;
		Array<VkPipelineShaderStageCreateInfo, SHADER_TYPE_MAX> m_shader_stages;

		// swap chain
		VkSwapchainKHR m_swap_chain;
		Vector<VkImage> m_swap_chain_images;
		Vector<VkImageView> m_swap_chain_image_views;
		VkFormat m_swap_chain_image_format;
		VkExtent2D m_swap_chain_extent;
		Vector<VkFramebuffer> m_swap_chain_framebuffers;
		u32 m_curr_image_idx;

		// depth
		VulkanTexture m_depth;

#if WVN_DEBUG
		VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
	};
}

#endif // VK_BACKEND_H

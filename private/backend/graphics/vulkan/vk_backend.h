#ifndef VK_BACKEND_H_
#define VK_BACKEND_H_

#include <vulkan/vulkan.h>

#include <wvn/container/vector.h>
#include <wvn/container/array.h>
#include <wvn/container/optional.h>
#include <wvn/container/hash_map.h>

#include <wvn/root.h>
#include <wvn/common.h>
#include <wvn/const.h>

#include <wvn/graphics/renderer_backend.h>

#include <backend/graphics/vulkan/vk_render_pass_builder.h>

#include <backend/graphics/vulkan/vk_descriptor_pool_mgr.h>
#include <backend/graphics/vulkan/vk_descriptor_builder.h>
#include <backend/graphics/vulkan/vk_descriptor_cache.h>

#include <backend/graphics/vulkan/vk_ubo_manager.h>

#include <backend/graphics/vulkan/vk_buffer.h>
#include <backend/graphics/vulkan/vk_texture.h>
#include <backend/graphics/vulkan/vk_shader.h>
#include <backend/graphics/vulkan/vk_render_target.h>
#include <backend/graphics/vulkan/vk_backbuffer.h>

#include <backend/graphics/vulkan/vk_buffer_mgr.h>
#include <backend/graphics/vulkan/vk_texture_mgr.h>
#include <backend/graphics/vulkan/vk_shader_mgr.h>
#include <backend/graphics/vulkan/vk_render_target_mgr.h>

#include <wvn/maths/mat4x4.h>

namespace wvn::gfx
{
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

	class VulkanBackend : public RendererBackend
	{
		struct FrameData
		{
			VkFence in_flight_fence;
			VkCommandPool command_pool;
			VkCommandBuffer command_buffer;
		};

	public:
		VulkanBackend();
		~VulkanBackend() override;

		RendererBackendProperties properties() override;

		void begin_render() override;
		void render(const RenderOp& op) override;
		void end_render() override;

        Backbuffer* create_backbuffer() override;

		void swap_buffers() override;

		void set_render_target(RenderTarget* target) override;

		void on_window_resize(int width, int height) override;

		void toggle_blending(bool enabled) override;
		void set_blend_write_mask(bool r, bool g, bool b, bool a) override;
		void set_blend_colour(const Blend& blend) override;
		void set_blend_alpha(const Blend& blend) override;
		void set_blend_constants(float r, float g, float b, float a) override;
		void get_blend_constants(float* constants) override;
		void set_blend_op(bool enabled, LogicOp op) override;

		void set_depth_params(bool depth_test, bool depth_write) override;
		void set_depth_op(CompareOp op) override;
		void set_depth_bounds_test(bool enabled) override;
		void set_depth_bounds(float min, float max) override;
		void set_depth_stencil_test(bool enabled) override;

		void set_viewport(const RectF& rect) override;
		void set_scissor(const RectI& rect) override;

		void set_sample_shading(bool enabled, float min_sample_shading) override;
		void set_cull_mode(CullMode cull) override;

		void set_texture(u32 idx, const Texture* texture) override;
		void set_sampler(u32 idx, TextureSampler* sampler) override;

		void bind_shader(const ShaderProgram* shader) override;
		void bind_shader_params(ShaderProgramType shader_type, ShaderParameters& params) override;

		void set_push_constants(ShaderParameters& params) override;
		void reset_push_constants() override;

		FrameData& current_frame();
		u64 get_current_frame_idx() const;

		void sync_stall() const;

		void clear_descriptor_set_and_pool();

        VkInstance vulkan_instance;
		VkDevice device;
		PhysicalDeviceData physical_data;
		QueueData queues;
		FrameData frames[vkutil::FRAMES_IN_FLIGHT];
		VkSampleCountFlagBits msaa_samples;
		VkFormat swap_chain_image_format;

	private:
		void enumerate_physical_devices();
		void create_logical_device(const QueueFamilyIdx& phys_idx);
		void create_command_pools(u32 graphics_family_idx);
		void create_command_buffers();
		void create_pipeline_process_cache();
		VkSampleCountFlagBits get_max_usable_sample_count() const;

		void clear_pipeline_cache();

		VkPipeline get_graphics_pipeline();
		VkPipelineLayout get_graphics_pipeline_layout();

		void reset_descriptor_builder();

		VulkanDescriptorBuilder get_descriptor_builder();
		VkDescriptorSet get_descriptor_set();

		// core
		u64 m_current_frame_idx;

		// managers
		VulkanBufferMgr* m_buffer_mgr;
		VulkanTextureMgr* m_texture_mgr;
		VulkanShaderMgr* m_shader_mgr;
		VulkanRenderTargetMgr* m_render_target_mgr;

		// pipeline
		HashMap<u64, VkPipeline> m_pipeline_cache;
		HashMap<u64, VkPipelineLayout> m_pipeline_layout_cache;
		VkPipelineCache m_pipeline_process_cache;

		// render pass
		VulkanRenderPassBuilder* m_current_render_pass_builder;
		Array<VkDescriptorImageInfo, wvn_MAX_BOUND_TEXTURES> m_image_infos;
		Array<VkPipelineShaderStageCreateInfo, SHADER_TYPE_GRAPHICS_COUNT> m_shader_stages;

		// descriptors
		VulkanDescriptorPoolMgr m_descriptor_pool_mgr;
		VulkanDescriptorCache m_descriptor_cache;
		VulkanDescriptorBuilder m_descriptor_builder;
		bool m_descriptor_builder_dirty;

		// swap chain
        VulkanBackbuffer* m_backbuffer;
        VkViewport m_viewport;
		VkRect2D m_scissor;

		// shader parameters
		VulkanUBOManager m_ubo_mgr;
		//Array<const ShaderParameters*, SHADER_TYPE_GRAPHICS_COUNT> m_current_shader_parameters;
		ShaderParameters::PackedConstants m_push_constants;

		// rendering configs
		VkPipelineDepthStencilStateCreateInfo m_depth_stencil_create_info;
		VkPipelineColorBlendAttachmentState m_colour_blend_attachment_state;
		float m_blend_constants[4];
		bool m_sample_shading_enabled;
		float m_min_sample_shading;
		bool m_blend_state_logic_op_enabled;
		VkLogicOp m_blend_state_logic_op;
		VkCullModeFlagBits m_cull_mode;

#if wvn_DEBUG
		VkDebugUtilsMessengerEXT m_debug_messenger;
#endif // wvn_DEBUG
	};
}

#endif // VK_BACKEND_H_

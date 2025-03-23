#ifndef UBO_MANAGER_H_
#define UBO_MANAGER_H_

#include <wvn/common.h>
#include <wvn/container/array.h>

#include <wvn/graphics/shader.h>

#include <backend/graphics/vulkan/vk_util.h>
#include <backend/graphics/vulkan/vk_buffer.h>

namespace wvn::gfx
{
	/**
	 * Abstracts away & manages the allocation
	 * and lifetime of a uniform buffer for use in shaders
	 */
	class VulkanUBOManager
	{
	public:
		VulkanUBOManager();

		void init(VulkanBackend* backend, u64 initial_size);
		void clean_up();

		void push_data(ShaderProgramType shader, const void* data, u64 size, bool* modified);
		void reallocate_uniform_buffer(u64 size);

		void reset_ubo_usage_in_frame();

		u64 get_descriptor_count() const;
		const VkDescriptorBufferInfo& get_descriptor(u64 idx) const;
		const Array<VkDescriptorBufferInfo, SHADER_TYPE_GRAPHICS_COUNT>& get_descriptors() const;

		const Array<u32, SHADER_TYPE_GRAPHICS_COUNT>& get_dynamic_offsets() const;

	private:
		VulkanBackend* m_backend;

		VulkanBuffer* m_ubo;
		Array<u32, SHADER_TYPE_GRAPHICS_COUNT> m_ubo_dynamic_offsets;
		Array<VkDescriptorBufferInfo, SHADER_TYPE_GRAPHICS_COUNT> m_ubo_infos;
		Array<u64, vkutil::FRAMES_IN_FLIGHT> m_ubo_usage_in_frame;
		u64 m_ubo_offset;
		u64 m_ubo_max_size;
	};
}

#endif // UBO_MANAGER_H_

#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include <wvn/graphics/gpu_buffer.h>
#include <vulkan/vulkan.h>
#include <wvn/common.h>

namespace wvn::gfx
{
	class VulkanTexture;
	class VulkanBackend;

	class VulkanBuffer : public GPUBuffer
	{
	public:
		VulkanBuffer(GPUBufferUsage usage);
		~VulkanBuffer() override;

		void create(VulkanBackend* backend, VkMemoryPropertyFlags properties, u64 size);
		void clean_up();

		void read_data(const void* src, u64 length, u64 offset) override;
		void write_data(void* dst, u64 length, u64 offset) override;
		void write_to(const GPUBuffer* other, u64 length, u64 src_offset, u64 dst_offset) override;
		void write_to_tex(const Texture* texture, u64 size, u64 offset = 0, u32 base_array_layer = 0) override;

		VkBuffer buffer() const;
		VkDeviceMemory memory() const;

		GPUBufferUsage usage() const override;
		VkMemoryPropertyFlags properties() const;

	private:
		VulkanBackend* m_backend;

		VkBuffer m_buffer;
		VkDeviceMemory m_memory;

		GPUBufferUsage m_usage;
		VkMemoryPropertyFlags m_properties;
	};
}

#endif // VK_BUFFER_H

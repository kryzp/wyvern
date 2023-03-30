#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include <wvn/graphics/gpu_buffer.h>
#include <vulkan/vulkan.h>
#include <wvn/util/types.h>

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

		void read_data(void* dst, u64 length) override;
		void write_data(const void* src, u64 length) override;
		void write_to(const GPUBuffer* other, u64 length) override;
		void write_to_tex(const Texture* texture, u64 size) override;

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

#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include <wvn/graphics/gpu_buffer.h>
#include <vulkan/vulkan.h>
#include <wvn/util/types.h>

namespace wvn::gfx
{
	class VulkanTexture;

	class VulkanBuffer : public GPUBuffer
	{
	public:
		VulkanBuffer(GPUBufferUsage usage);
		~VulkanBuffer() override;

		void create(VkDevice device, VkPhysicalDevice physical_device, VkMemoryPropertyFlags properties, VkCommandPool command_pool, VkQueue graphics_queue, u64 size);
		void clean_up();

		void read_data(void* dst, u64 length) override;
		void write_data(const void* src, u64 length) override;
		void write_to(const GPUBuffer* other, u64 length) override;

		void copy_to_texture(const VulkanTexture& texture, VkCommandPool cmd_pool, VkQueue graphics);

		VkDevice device() const;
		VkPhysicalDevice physical_device() const;

		VkBuffer buffer() const;
		VkDeviceMemory memory() const;

		GPUBufferUsage usage() const override;
		VkMemoryPropertyFlags properties() const;

	private:
		VkDevice m_device;
		VkPhysicalDevice m_physical_device;
		VkCommandPool m_command_pool;
		VkQueue m_graphics_queue;

		VkBuffer m_buffer;
		VkDeviceMemory m_memory;

		GPUBufferUsage m_usage;
		VkMemoryPropertyFlags m_properties;
	};
}

#endif // VK_BUFFER_H

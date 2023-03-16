#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include <vulkan/vulkan.h>
#include <wvn/util/types.h>

namespace wvn::gfx
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer();
		~VulkanBuffer();

		void create(VkDevice device, VkPhysicalDevice physical_device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
		void clean_up();

		void send_data(const void* data);

		void copy_to(const VulkanBuffer& other, VkCommandPool cmd_pool, VkQueue graphics_queue);
		void copy_to_image(VkImage image, u32 width, u32 height, VkCommandPool cmd_pool, VkQueue graphics_queue);

		VkDevice device() const;
		VkPhysicalDevice physical_device() const;

		VkBuffer buffer() const;
		VkDeviceMemory memory() const;

		VkDeviceSize size() const;
		VkBufferUsageFlags usage() const;
		VkMemoryPropertyFlags properties() const;

	private:
		VkDevice m_device;
		VkPhysicalDevice m_physical_device;

		VkBuffer m_buffer;
		VkDeviceMemory m_memory;

		VkDeviceSize m_size;
		VkBufferUsageFlags m_usage;
		VkMemoryPropertyFlags m_properties;
	};
}

#endif // VK_BUFFER_H

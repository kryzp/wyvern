#ifndef VK_IMAGE_H
#define VK_IMAGE_H

#include <vulkan/vulkan.h>
#include <wvn/graphics/texture.h>
#include <wvn/util/types.h>

namespace wvn::gfx
{
	class VulkanImage
	{
	public:
		VulkanImage();
		~VulkanImage();

		void clean_up(VkDevice device);

		void create(VkDevice device, VkPhysicalDevice physical_device, VkMemoryPropertyFlags properties, u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);

		void transition_layout(VkFormat fmt, VkImageLayout new_layout, VkCommandPool cmd_pool, VkDevice device, VkQueue graphics);

		VkImage& image();
		VkDeviceMemory memory() const;
		VkImageLayout layout() const;

	private:
		VkImage m_image;
		VkDeviceMemory m_memory;
		VkImageLayout m_layout;
	};
}

#endif // VK_IMAGE_H

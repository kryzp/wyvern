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
		VulkanImage(VkDevice device, VkPhysicalDevice physical_device, VkMemoryPropertyFlags properties, u32 width, u32 height, TextureFormat format, TextureTiling tiling);
		~VulkanImage();

		void init(VkDevice device, VkPhysicalDevice physical_device);
		void clean_up();

		void create(VkMemoryPropertyFlags properties, u32 width, u32 height, VkFormat format, VkImageTiling tiling);

		void transition_layout(VkFormat fmt, VkImageLayout new_layout, VkCommandPool cmd_pool, VkQueue graphics_queue);

		VkImage& image();
		VkDeviceMemory memory() const;
		VkImageLayout layout() const;

	private:
		VkDevice m_device;
		VkPhysicalDevice m_physical_device;

		VkImage m_image;
		VkDeviceMemory m_memory;
		VkImageLayout m_layout;
	};
}

#endif // VK_IMAGE_H

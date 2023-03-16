#ifndef VK_IMAGE_VIEW_H
#define VK_IMAGE_VIEW_H

#include <vulkan/vulkan.h>
#include <wvn/graphics/texture.h>

namespace wvn::gfx
{
	class VulkanImageView
	{
	public:
		VulkanImageView();
		~VulkanImageView();

		void init(VkDevice device);
		void clean_up();

		void create(VkImage image, VkFormat format);

		VkImageView& view();

	private:
		VkDevice m_device;
		VkImageView m_view;
	};
}

#endif // VK_IMAGE_VIEW_H

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

		void clean_up();

		void create(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);

		VkImageView& view();

	private:
		VkImageView m_view;
	};
}

#endif // VK_IMAGE_VIEW_H

#include <backend/graphics/vulkan/vk_image_view.h>
#include <backend/graphics/vulkan/vk_util.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanImageView::VulkanImageView()
	: m_device(VK_NULL_HANDLE)
	, m_view(VK_NULL_HANDLE)
{
}

VulkanImageView::~VulkanImageView()
{
}

void VulkanImageView::init(VkDevice device)
{
	this->m_device = device;
}

void VulkanImageView::clean_up()
{
	if (m_view == VK_NULL_HANDLE) {
		return;
	}

	vkDestroyImageView(m_device, m_view, nullptr);

	m_view = VK_NULL_HANDLE;
}

void VulkanImageView::create(VkImage image, VkFormat format)
{
	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = image;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = format;

	view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;

	view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	if (VkResult result = vkCreateImageView(m_device, &view_info, nullptr, &m_view); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton()->print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN|DEBUG] Failed to create texture image view.");
	}
}

VkImageView& VulkanImageView::view() { return m_view; }

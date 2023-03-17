#include <backend/graphics/vulkan/vk_image_view.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanImageView::VulkanImageView()
	: m_view(VK_NULL_HANDLE)
{
}

VulkanImageView::~VulkanImageView()
{
}

void VulkanImageView::clean_up()
{
	if (m_view == VK_NULL_HANDLE) {
		return;
	}

	vkDestroyImageView(static_cast<VulkanBackend*>(Root::get_singleton()->renderer_backend())->logical_data().device, m_view, nullptr);

	m_view = VK_NULL_HANDLE;
}

void VulkanImageView::create(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags)
{
	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = image;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = format;

	view_info.subresourceRange.aspectMask = aspect_flags;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;

	view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	if (VkResult result = vkCreateImageView(device, &view_info, nullptr, &m_view); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton()->print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN|DEBUG] Failed to create texture image view.");
	}
}

VkImageView& VulkanImageView::view() { return m_view; }

#include <backend/graphics/vulkan/vk_image.h>
#include <backend/graphics/vulkan/vk_util.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanImage::VulkanImage()
	: m_device(VK_NULL_HANDLE)
	, m_physical_device(VK_NULL_HANDLE)
	, m_image(VK_NULL_HANDLE)
	, m_memory(VK_NULL_HANDLE)
	, m_layout(VK_IMAGE_LAYOUT_UNDEFINED)
{
}

VulkanImage::VulkanImage(
	VkDevice device, VkPhysicalDevice physical_device,
	VkMemoryPropertyFlags properties,
	u32 width, u32 height,
	TextureFormat format, TextureTiling tiling
)
	: m_device(device)
	, m_physical_device(physical_device)
	, m_image(VK_NULL_HANDLE)
	, m_memory(VK_NULL_HANDLE)
	, m_layout(VK_IMAGE_LAYOUT_UNDEFINED)
{
	create(properties, width, height, vkutil::get_vk_texture_format(format), vkutil::get_vk_texture_tile(tiling));
}

VulkanImage::~VulkanImage()
{
}

void VulkanImage::init(VkDevice device, VkPhysicalDevice physical_device)
{
	this->m_device = device;
	this->m_physical_device = physical_device;
}

void VulkanImage::clean_up()
{
	if (m_image == VK_NULL_HANDLE &&
		m_memory == VK_NULL_HANDLE)
	{
		return;
	}

	vkDestroyImage(m_device, m_image, nullptr);
	vkFreeMemory(m_device, m_memory, nullptr);

	m_image = VK_NULL_HANDLE;
	m_memory = VK_NULL_HANDLE;
}

void VulkanImage::create(
	VkMemoryPropertyFlags properties,
	u32 width, u32 height,
	VkFormat format, VkImageTiling tiling
)
{
	VkImageCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	create_info.imageType = VK_IMAGE_TYPE_2D;
	create_info.extent.width = width;
	create_info.extent.height = height;
	create_info.extent.depth = 1;
	create_info.mipLevels = 1;
	create_info.arrayLayers = 1;
	create_info.format = format;
	create_info.tiling = tiling;
	create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	create_info.samples = VK_SAMPLE_COUNT_1_BIT;
	create_info.flags = 0;

	if (VkResult result = vkCreateImage(m_device, &create_info, nullptr, &m_image); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton()->print("[VULKAN:IMAGE] Result: %d", result);
		WVN_ERROR("[VULKAN:IMAGE|DEBUG] Failed to create command pool.");
	}

	VkMemoryRequirements memory_requirements = {};
	vkGetImageMemoryRequirements(m_device, m_image, &memory_requirements);

	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = memory_requirements.size;
	alloc_info.memoryTypeIndex = vkutil::find_memory_type(m_physical_device, memory_requirements.memoryTypeBits, properties);

	if (VkResult result = vkAllocateMemory(m_device, &alloc_info, nullptr, &m_memory); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton()->print("[VULKAN:IMAGE] Result: %d", result);
		WVN_ERROR("[VULKAN:IMAGE|DEBUG] Failed to allocate memory for image.");
	}

	vkBindImageMemory(m_device, m_image, m_memory, 0);

	dev::LogMgr::get_singleton()->print("[VULKAN:IMAGE] Created!");
}

void VulkanImage::transition_layout(VkFormat fmt, VkImageLayout new_layout, VkCommandPool cmd_pool, VkQueue graphics_queue)
{
	VkCommandBuffer cmd_buf = vkutil::begin_single_time_commands(m_device, cmd_pool);
	{
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = m_layout;
		barrier.newLayout = new_layout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = 0;

		VkPipelineStageFlags src_stage = VK_PIPELINE_STAGE_NONE;
		VkPipelineStageFlags dst_stage = VK_PIPELINE_STAGE_NONE;

		if (m_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (m_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			WVN_ERROR("[VULKAN:IMAGE|DEBUG] Unsupported layout transition for image.");
		}

		vkCmdPipelineBarrier(
			cmd_buf,
			src_stage, dst_stage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		m_layout = new_layout;
	}
	vkutil::end_single_time_commands(m_device, cmd_pool, cmd_buf, graphics_queue);
}

VkImage& VulkanImage::image() { return m_image; }
VkDeviceMemory VulkanImage::memory() const { return m_memory; }
VkImageLayout VulkanImage::layout() const { return m_layout; }

#include <backend/graphics/vulkan/vk_texture.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <backend/graphics/vulkan/vk_util.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanTexture::VulkanTexture(VulkanBackend* backend)
	: m_backend(backend)
	, m_image(VK_NULL_HANDLE)
	, m_image_memory(VK_NULL_HANDLE)
	, m_image_layout()
	, m_view(VK_NULL_HANDLE)
	, m_format()
	, m_tiling()
	, m_width(0)
	, m_height(0)
	, m_mip_levels(0)
	, m_num_samples(VK_SAMPLE_COUNT_1_BIT)
	, m_transient(false)
{
}

VulkanTexture::~VulkanTexture()
{
	clean_up();
}

void VulkanTexture::clean_up()
{
	if (m_image != VK_NULL_HANDLE)
	{
		vkDestroyImage(m_backend->device, m_image, nullptr);
		m_image = VK_NULL_HANDLE;
	}

	if (m_image_memory != VK_NULL_HANDLE)
	{
		vkFreeMemory(m_backend->device, m_image_memory, nullptr);
		m_image_memory = VK_NULL_HANDLE;
	}

	if (m_view != VK_NULL_HANDLE)
	{
		vkDestroyImageView(m_backend->device, m_view, nullptr);
		m_view = VK_NULL_HANDLE;
	}
}

void VulkanTexture::create(const Image& image, u32 mip_levels, VkSampleCountFlagBits num_samples, bool transient)
{
	create(image.width(), image.height(), TEX_FMT_R8G8B8A8_SRGB, TEX_TILE_OPTIMAL, mip_levels, num_samples, transient);
}

void VulkanTexture::create(u32 width, u32 height, TextureFormat format, TextureTiling tiling, u32 mip_levels, VkSampleCountFlagBits num_samples, bool transient) // todo: get rid of the bool transient
{
	this->m_width = width;
	this->m_height = height;

	this->m_format = format;
	this->m_tiling = tiling;

	this->m_mip_levels = mip_levels;
	this->m_num_samples = num_samples;
	this->m_transient = transient;

	create_internal_resources();
}

void VulkanTexture::create_internal_resources()
{
	VkFormat vkfmt = vkutil::get_vk_texture_format(m_format);
	VkImageTiling vktile = vkutil::get_vk_texture_tile(m_tiling);

	VkImageCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	create_info.imageType = VK_IMAGE_TYPE_2D;
	create_info.extent.width = m_width;
	create_info.extent.height = m_height;
	create_info.extent.depth = 1;
	create_info.mipLevels = m_mip_levels;
	create_info.arrayLayers = 1;
	create_info.format = vkfmt;
	create_info.tiling = vktile;
	create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	create_info.usage = m_transient ? VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT : VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	create_info.samples = m_num_samples;
	create_info.flags = 0;

	if (vkutil::has_stencil_component(vkfmt)) {
		create_info.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	} else {
		create_info.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}

	if (VkResult result = vkCreateImage(m_backend->device, &create_info, nullptr, &m_image); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton()->print("[VULKAN:TEXTURE] Result: %d", result);
		WVN_ERROR("[VULKAN:TEXTURE|DEBUG] Failed to create command pool.");
	}

	VkMemoryRequirements memory_requirements = {};
	vkGetImageMemoryRequirements(m_backend->device, m_image, &memory_requirements);

	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = memory_requirements.size;
	alloc_info.memoryTypeIndex = vkutil::find_memory_type(m_backend->physical_data.device, memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (VkResult result = vkAllocateMemory(m_backend->device, &alloc_info, nullptr, &m_image_memory); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton()->print("[VULKAN:TEXTURE] Result: %d", result);
		WVN_ERROR("[VULKAN:TEXTURE|DEBUG] Failed to allocate memory for image.");
	}

	vkBindImageMemory(m_backend->device, m_image, m_image_memory, 0);

	m_view = generate_view();
}

VkImageView VulkanTexture::generate_view() const
{
	VkFormat vkfmt = vkutil::get_vk_texture_format(m_format);

	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = m_image;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = vkfmt;

	view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = m_mip_levels;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;

	if (vkutil::has_stencil_component(vkfmt)) {
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	VkImageView ret = {};

	if (VkResult result = vkCreateImageView(m_backend->device, &view_info, nullptr, &ret); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton()->print("[VULKAN:TEXTURE] Result: %d", result);
		WVN_ERROR("[VULKAN|DEBUG:TEXTURE] Failed to create texture image view.");
	}

	return ret;
}

// requires texture to be in TRANSFER_DST_OPTIMAL layout
// also transitions the texture into SHADER_READ_ONLY layout
void VulkanTexture::generate_mipmaps()
{
	VkFormat vkfmt = vkutil::get_vk_texture_format(m_format);

	VkFormatProperties format_properties = {};
	vkGetPhysicalDeviceFormatProperties(m_backend->physical_data.device, vkfmt, &format_properties);

	if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
		WVN_ERROR("[VULKAN|DEBUG:TEXTURE] Texture image format doesn't support linear blitting.");
		return;
	}

	VkCommandBuffer cmd_buf = vkutil::begin_single_time_commands(m_backend->frames[m_backend->frame()].command_pool, m_backend->device);
	{
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = m_image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int mip_width = m_width;
		int mip_height = m_height;

		for (int i = 1; i < m_mip_levels; i++)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(
				cmd_buf,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);

			VkImageBlit blit = {};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mip_width, mip_height, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { (mip_width > 1) ? mip_width / 2 : 1, (mip_height > 1) ? mip_height / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(
				cmd_buf,
				m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR
			);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(
				cmd_buf,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);

			if (mip_width > 1) {
				mip_width /= 2;
			}

			if (mip_height > 1) {
				mip_height /= 2;
			}
		}

		barrier.subresourceRange.baseMipLevel = m_mip_levels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(
			cmd_buf,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	}
	vkutil::end_single_time_commands(m_backend->frames[m_backend->frame()].command_pool, cmd_buf, m_backend->device, m_backend->queues.graphics);
}

void VulkanTexture::transition_layout(VkImageLayout new_layout)
{
	VkCommandBuffer cmd_buf = vkutil::begin_single_time_commands(m_backend->frames[m_backend->frame()].command_pool, m_backend->device);
	{
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = m_image_layout;
		barrier.newLayout = new_layout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = m_mip_levels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = 0;

		if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		VkPipelineStageFlags src_stage = VK_PIPELINE_STAGE_NONE;
		VkPipelineStageFlags dst_stage = VK_PIPELINE_STAGE_NONE;

		if (m_image_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (m_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (m_image_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dst_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
		{
			WVN_ERROR("[VULKAN:TEXTURE|DEBUG] Unsupported layout transition for image.");
		}

		vkCmdPipelineBarrier(
			cmd_buf,
			src_stage, dst_stage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		m_image_layout = new_layout;
	}
	vkutil::end_single_time_commands(m_backend->frames[m_backend->frame()].command_pool, cmd_buf, m_backend->device, m_backend->queues.graphics);
}

TextureMetaData VulkanTexture::meta_data() const { return {
	.format = m_format,
	.tiling = m_tiling
}; }

VkImage VulkanTexture::image() const { return m_image; }
VkImageView VulkanTexture::image_view() const { return m_view; }
u32 VulkanTexture::width() const { return m_width; }
u32 VulkanTexture::height() const { return m_height; }
u32 VulkanTexture::mip_levels() const { return m_mip_levels; }
VkSampleCountFlagBits VulkanTexture::num_samples() const { return m_num_samples; }
bool VulkanTexture::transient() const { return m_transient; }

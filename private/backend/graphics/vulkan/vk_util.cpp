#include <backend/graphics/vulkan/vk_util.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/util/assert.h>

using namespace wvn;
using namespace wvn::gfx;

u32 vkutil::find_memory_type(VkPhysicalDevice physical_device, u32 filter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memory_properties = {};
	vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

	for (u32 i = 0; i < memory_properties.memoryTypeCount; i++) {
		if ((filter & (1 << i)) && ((memory_properties.memoryTypes[i].propertyFlags & properties) == properties)) {
			return i;
		}
	}

	WVN_ERROR("[VULKAN|DEBUG] Failed to find suitable memory type.");
	return 0;
}

VkFormat vkutil::get_vk_texture_format(TextureFormat fmt)
{
	switch (fmt)
	{
		case TEX_FMT_R8G8B8A8_SRGB:
			return VK_FORMAT_R8G8B8A8_SRGB;
		default:
			return VK_FORMAT_MAX_ENUM;
	}
}

VkImageTiling vkutil::get_vk_texture_tile(TextureTiling tiling)
{
	switch (tiling)
	{
		case TEX_TILE_LINEAR:
			return VK_IMAGE_TILING_LINEAR;
		case TEX_TILE_OPTIMAL:
			return VK_IMAGE_TILING_OPTIMAL;
		default:
			return VK_IMAGE_TILING_MAX_ENUM;
	}
}

VkFilter vkutil::get_vk_filter(TextureFilter filter)
{
	switch (filter)
	{
		case TEX_FILTER_LINEAR:
			return VK_FILTER_LINEAR;
		case TEX_FILTER_NEAREST:
			return VK_FILTER_NEAREST;
		default:
			return VK_FILTER_MAX_ENUM;
	}
}

VkSamplerAddressMode vkutil::get_vk_address_mode(TextureWrap wrap)
{
	switch (wrap)
	{
		case TEX_WRAP_REPEAT:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case TEX_WRAP_CLAMP:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		default:
			return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
	}
}

VkCommandBuffer vkutil::begin_single_time_commands(VkDevice device, VkCommandPool cmd_pool)
{
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = cmd_pool;
	alloc_info.commandBufferCount = 1;

	VkCommandBuffer cmd_buf = {};

	if (VkResult result = vkAllocateCommandBuffers(device, &alloc_info, &cmd_buf); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton()->print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN|DEBUG] Failed to realloc command buffers when copying buffer.");
	}

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (VkResult result = vkBeginCommandBuffer(cmd_buf, &begin_info); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton()->print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN|DEBUG] Failed to begin command buffer when copying buffer.");
	}

	return cmd_buf;
}

void vkutil::end_single_time_commands(VkDevice device, VkCommandPool cmd_pool, VkCommandBuffer cmd_buf, VkQueue graphics_queue)
{
	vkEndCommandBuffer(cmd_buf);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &cmd_buf;

	vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphics_queue);

	vkFreeCommandBuffers(device, cmd_pool, 1, &cmd_buf);
}

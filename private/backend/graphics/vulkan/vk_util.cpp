#include <backend/graphics/vulkan/vk_util.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <wvn/system/system_backend.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/util/assert.h>

using namespace wvn;
using namespace wvn::gfx;

VkSurfaceFormatKHR vkutil::choose_swap_surface_format(const Vector<VkSurfaceFormatKHR>& available_surface_formats)
{
	// todo: this could be ranked based on how good each format is rather than choosing the first one that fits

	for (auto& available_format : available_surface_formats)
	{
		if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return available_format;
		}
	}

	return available_surface_formats[0];
}

VkPresentModeKHR vkutil::choose_swap_present_mode(const Vector<VkPresentModeKHR>& available_present_modes)
{
	for (const auto& available_present_mode : available_present_modes)
	{
		if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return available_present_mode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D vkutil::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != CalcU::max_value())
	{
		return capabilities.currentExtent;
	}
	else
	{
		Vec2I wh = Root::get_singleton()->system_backend()->get_window_size();
		VkExtent2D actual_extent = { static_cast<u32>(wh.x), static_cast<u32>(wh.y) };

		actual_extent.width  = CalcU::clamp(actual_extent.width,  capabilities.minImageExtent.width,  capabilities.maxImageExtent.width );
		actual_extent.height = CalcU::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actual_extent;
	}
}

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

VkFormat vkutil::find_supported_format(VkPhysicalDevice device, const Vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (const auto& fmt : candidates)
	{
		VkFormatProperties properties = {};
		vkGetPhysicalDeviceFormatProperties(device, fmt, &properties);

		if ((tiling == VK_IMAGE_TILING_LINEAR  && (properties.linearTilingFeatures  & features) == features) ||
			(tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features))
		{
			return fmt;
		}
	}

	WVN_ERROR("[VULKAN:UTIL|DEBUG] Failed to find supported format.");
	return VK_FORMAT_MAX_ENUM;
}

VkFormat vkutil::find_depth_format(VkPhysicalDevice device)
{
	return find_supported_format(
		device,
		{
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT
		},
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool vkutil::has_stencil_component(VkFormat format)
{
	return (format == VK_FORMAT_D32_SFLOAT) || (format == VK_FORMAT_D32_SFLOAT_S8_UINT) || (format == VK_FORMAT_D24_UNORM_S8_UINT);
}

VkFormat vkutil::get_vk_texture_format(TextureFormat fmt)
{
	switch (fmt)
	{
		case TEX_FMT_R8G8B8A8_SRGB:
			return VK_FORMAT_R8G8B8A8_SRGB;
		case TEX_FMT_R8_UNORM:
			return VK_FORMAT_R8_UNORM;
		case TEX_FMT_D32_SFLOAT:
			return VK_FORMAT_D32_SFLOAT;
		case TEX_FMT_D32_SFLOAT_S8_UINT:
			return VK_FORMAT_D32_SFLOAT_S8_UINT;
		case TEX_FMT_D24_UNORM_S8_UINT:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		default:
			WVN_ERROR("[VULKAN:UTIL|DEBUG] Failed to find VkFormat given TextureFormat.");
			return VK_FORMAT_MAX_ENUM;
	}
}

TextureFormat vkutil::get_wvn_texture_format(VkFormat fmt)
{
	switch (fmt)
	{
		case VK_FORMAT_R8G8B8A8_SRGB:
			return TEX_FMT_R8G8B8A8_SRGB;
		case VK_FORMAT_D32_SFLOAT:
			return TEX_FMT_D32_SFLOAT;
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return TEX_FMT_D32_SFLOAT_S8_UINT;
		case VK_FORMAT_D24_UNORM_S8_UINT:
			return TEX_FMT_D24_UNORM_S8_UINT;
		default:
		WVN_ERROR("[VULKAN:UTIL|DEBUG] Failed to find TextureFormat given VkFormat.");
			return TEX_FMT_MAX;
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
			WVN_ERROR("[VULKAN:UTIL|DEBUG] Failed to find VkImageTiling given TextureTiling.");
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
			WVN_ERROR("[VULKAN:UTIL|DEBUG] Failed to find VkFilter given TextureFilter.");
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
			WVN_ERROR("[VULKAN:UTIL|DEBUG] Failed to find VkSamplerAddressMode given TextureWrap.");
			return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
	}
}

VkShaderStageFlagBits vkutil::get_vk_shader_flag_bits(ShaderType type)
{
	switch (type)
	{
		case SHADER_TYPE_VERTEX:
			return VK_SHADER_STAGE_VERTEX_BIT;
		case SHADER_TYPE_FRAGMENT:
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		case SHADER_TYPE_GEOMETRY:
			return VK_SHADER_STAGE_GEOMETRY_BIT;
		case SHADER_TYPE_COMPUTE:
			return VK_SHADER_STAGE_COMPUTE_BIT;
		default:
			WVN_ERROR("[VULKAN:UTIL|DEBUG] Failed to find VkShaderStageFlagBits given ShaderType.");
			return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	}
}

VkCommandBuffer vkutil::begin_single_time_commands(VkCommandPool cmd_pool, VkDevice device)
{
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = cmd_pool;
	alloc_info.commandBufferCount = 1;

	VkCommandBuffer cmd_buf = {};

	if (VkResult result = vkAllocateCommandBuffers(device, &alloc_info, &cmd_buf); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton()->print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:UTIL|DEBUG] Failed to reallocate command buffers when copying buffer.");
	}

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (VkResult result = vkBeginCommandBuffer(cmd_buf, &begin_info); result != VK_SUCCESS) {
		dev::LogMgr::get_singleton()->print("[VULKAN] Result: %d", result);
		WVN_ERROR("[VULKAN:UTIL|DEBUG] Failed to begin command buffer when copying buffer.");
	}

	return cmd_buf;
}

void vkutil::end_single_time_commands(VkCommandPool cmd_pool, VkCommandBuffer cmd_buf, VkDevice device, VkQueue graphics)
{
	vkEndCommandBuffer(cmd_buf);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &cmd_buf;

	vkQueueSubmit(graphics, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphics);

	vkFreeCommandBuffers(device, cmd_pool, 1, &cmd_buf);
}

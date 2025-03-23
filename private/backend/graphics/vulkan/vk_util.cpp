#include <backend/graphics/vulkan/vk_util.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <wvn/system/system_backend.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

// todo: this could be ranked based on how good each format is rather than choosing the first one that fits
VkSurfaceFormatKHR vkutil::choose_swap_surface_format(const Vector<VkSurfaceFormatKHR>& available_surface_formats)
{
	for (auto& available_format : available_surface_formats) {
		if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return available_format;
		}
	}

	return available_surface_formats[0];
}

VkPresentModeKHR vkutil::choose_swap_present_mode(const Vector<VkPresentModeKHR>& available_present_modes)
{
	for (const auto& available_present_mode : available_present_modes) {
		if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
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
		VkExtent2D actual_extent = { (u32)wh.x, (u32)wh.y };

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

	wvn_ERROR("[VULKAN:UTIL|DEBUG] Failed to find suitable memory type.");
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

	wvn_ERROR("[VULKAN:UTIL|DEBUG] Failed to find supported format.");
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

VkCommandBuffer vkutil::begin_single_time_commands(VkCommandPool cmd_pool, VkDevice device)
{
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = cmd_pool;
	alloc_info.commandBufferCount = 1;

	VkCommandBuffer cmd_buf = {};

	if (VkResult result = vkAllocateCommandBuffers(device, &alloc_info, &cmd_buf); result != VK_SUCCESS) {
		wvn_ERROR("[VULKAN:UTIL|DEBUG] Failed to reallocate command buffers when copying buffer: %d", result);
	}

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (VkResult result = vkBeginCommandBuffer(cmd_buf, &begin_info); result != VK_SUCCESS) {
		wvn_ERROR("[VULKAN:UTIL|DEBUG] Failed to begin command buffer when copying buffer: %d", result);
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

void vkutil::end_single_time_graphics_commands(VulkanBackend* backend, VkCommandBuffer cmd_buf)
{
	vkutil::end_single_time_commands(backend->current_frame().command_pool, cmd_buf, backend->device, backend->queues.graphics);
}

VkDeviceSize vkutil::get_ubo_size(u64 object_size, VkPhysicalDeviceProperties properties)
{
	const VkDeviceSize& minimum_size = properties.limits.minUniformBufferOffsetAlignment;
	return (object_size / minimum_size) * minimum_size + ((object_size % minimum_size) > 0 ? minimum_size : 0);
}

u32 vkutil::calc_ubo_aligned_offset(u64 offset, VkPhysicalDeviceProperties properties)
{
	const VkDeviceSize& minimum_size = properties.limits.minUniformBufferOffsetAlignment;
	return (offset / minimum_size) * minimum_size + ((offset % minimum_size) > 0 ? minimum_size : 0);
}

SwapChainSupportDetails vkutil::query_swap_chain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
	SwapChainSupportDetails result = {};

	// get capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &result.capabilities);

	// get surface formats
	u32 surf_fmt_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surf_fmt_count, nullptr);

	if (surf_fmt_count)
	{
		result.surface_formats.resize(surf_fmt_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surf_fmt_count, result.surface_formats.data());
	}

	// get present modes
	u32 present_mode_count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);

	if (present_mode_count)
	{
		result.present_modes.resize(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, result.present_modes.data());
	}

	return result;
}

QueueFamilyIdx vkutil::find_queue_families(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
	QueueFamilyIdx result = {};

	u32 queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

	if (!queue_family_count) {
		wvn_ERROR("[VULKAN:UTIL|DEBUG] Failed to find any queue families!");
	}

	Vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

	for (int i = 0; i < queue_family_count; i++)
	{
		if ((queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !result.graphics_family.has_value()) {
			result.graphics_family = i;
			continue;
		}

		if ((queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && !result.compute_family.has_value()) {
			result.compute_family = i;
			continue;
		}

		if ((queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && !result.transfer_family.has_value()) {
			result.transfer_family = i;
			continue;
		}

		VkBool32 present_support = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);

		if (present_support) {
			result.present_family = i;
			continue;
		}

		if (result.is_complete()) {
			break;
		}
	}

	return result;
}

bool vkutil::check_device_extension_support(VkPhysicalDevice physical_device)
{
	u32 ext_count = 0;
	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &ext_count, nullptr);

	if (!ext_count) {
		wvn_ERROR("[VULKAN:UTIL|DEBUG] Failed to find any device extension properties!");
	}

	Vector<VkExtensionProperties> available_exts(ext_count);
	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &ext_count, available_exts.data());

	Vector<const char*> required_exts(DEVICE_EXTENSIONS, wvn_ARRAY_LENGTH(DEVICE_EXTENSIONS));

	for (const auto& available_extension : available_exts) {
		for (int i = 0; i < required_exts.size(); i++) {
			if (cstr::compare(available_extension.extensionName, required_exts[i])) {
				required_exts.erase(i);
			}
		}
	}

	return required_exts.empty();
}

u32 vkutil::assign_physical_device_usability(
	VkSurfaceKHR surface,
	VkPhysicalDevice physical_device,
	VkPhysicalDeviceProperties properties,
	VkPhysicalDeviceFeatures features,
	bool* essentials_completed
)
{
	u32 result_usability = 0;

	bool adequate_swap_chain = false;
	bool has_required_extensions = check_device_extension_support(physical_device);
	bool indices_complete = find_queue_families(physical_device, surface).is_complete();
	bool has_anisotropy = features.samplerAnisotropy;

	if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		result_usability += 4;
	} else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
		result_usability += 1;
	}

	if (features.geometryShader) {
		result_usability += 1;
	}

	if (has_anisotropy) {
		result_usability += 1;
	}

	if (has_required_extensions) {
		SwapChainSupportDetails swap_chain_support_details = query_swap_chain_support(physical_device, surface);
		adequate_swap_chain = swap_chain_support_details.surface_formats.any() && swap_chain_support_details.present_modes.any();
		result_usability += 1;
	}

	if (essentials_completed) {
		(*essentials_completed) = indices_complete && has_required_extensions && adequate_swap_chain && has_anisotropy;
	}

	return result_usability;
}

/*
 * The rest of the code below is just a bunch of long translation functions
 * that convert engine enums into vulkan enums
 */

TextureFormat vkutil::get_wvn_texture_format(VkFormat fmt)
{
	switch (fmt)
	{
	case VK_FORMAT_R4G4_UNORM_PACK8:
		return TEX_FORMAT_R4G4_UNORM_PACK8;

	case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
		return TEX_FORMAT_R4G4B4A4_UNORM_PACK16;

	case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
		return TEX_FORMAT_B4G4R4A4_UNORM_PACK16;

	case VK_FORMAT_R5G6B5_UNORM_PACK16:
		return TEX_FORMAT_R5G6B5_UNORM_PACK16;

	case VK_FORMAT_B5G6R5_UNORM_PACK16:
		return TEX_FORMAT_B5G6R5_UNORM_PACK16;

	case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
		return TEX_FORMAT_R5G5B5A1_UNORM_PACK16;

	case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
		return TEX_FORMAT_B5G5R5A1_UNORM_PACK16;

	case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
		return TEX_FORMAT_A1R5G5B5_UNORM_PACK16;

	case VK_FORMAT_R8_UNORM:
		return TEX_FORMAT_R8_UNORM;

	case VK_FORMAT_R8_SNORM:
		return TEX_FORMAT_R8_SNORM;

	case VK_FORMAT_R8_USCALED:
		return TEX_FORMAT_R8_USCALED;

	case VK_FORMAT_R8_SSCALED:
		return TEX_FORMAT_R8_SSCALED;

	case VK_FORMAT_R8_UINT:
		return TEX_FORMAT_R8_UINT;

	case VK_FORMAT_R8_SINT:
		return TEX_FORMAT_R8_SINT;

	case VK_FORMAT_R8_SRGB:
		return TEX_FORMAT_R8_SRGB;

	case VK_FORMAT_R8G8_UNORM:
		return TEX_FORMAT_R8G8_UNORM;

	case VK_FORMAT_R8G8_SNORM:
		return TEX_FORMAT_R8G8_SNORM;

	case VK_FORMAT_R8G8_USCALED:
		return TEX_FORMAT_R8G8_USCALED;

	case VK_FORMAT_R8G8_SSCALED:
		return TEX_FORMAT_R8G8_SSCALED;

	case VK_FORMAT_R8G8_UINT:
		return TEX_FORMAT_R8G8_UINT;

	case VK_FORMAT_R8G8_SINT:
		return TEX_FORMAT_R8G8_SINT;

	case VK_FORMAT_R8G8_SRGB:
		return TEX_FORMAT_R8G8_SRGB;

	case VK_FORMAT_R8G8B8_UNORM:
		return TEX_FORMAT_R8G8B8_UNORM;

	case VK_FORMAT_R8G8B8_SNORM:
		return TEX_FORMAT_R8G8B8_SNORM;

	case VK_FORMAT_R8G8B8_USCALED:
		return TEX_FORMAT_R8G8B8_USCALED;

	case VK_FORMAT_R8G8B8_SSCALED:
		return TEX_FORMAT_R8G8B8_SSCALED;

	case VK_FORMAT_R8G8B8_UINT:
		return TEX_FORMAT_R8G8B8_UINT;

	case VK_FORMAT_R8G8B8_SINT:
		return TEX_FORMAT_R8G8B8_SINT;

	case VK_FORMAT_R8G8B8_SRGB:
		return TEX_FORMAT_R8G8B8_SRGB;

	case VK_FORMAT_B8G8R8_UNORM:
		return TEX_FORMAT_B8G8R8_UNORM;

	case VK_FORMAT_B8G8R8_SNORM:
		return TEX_FORMAT_B8G8R8_SNORM;

	case VK_FORMAT_B8G8R8_USCALED:
		return TEX_FORMAT_B8G8R8_USCALED;

	case VK_FORMAT_B8G8R8_SSCALED:
		return TEX_FORMAT_B8G8R8_SSCALED;

	case VK_FORMAT_B8G8R8_UINT:
		return TEX_FORMAT_B8G8R8_UINT;

	case VK_FORMAT_B8G8R8_SINT:
		return TEX_FORMAT_B8G8R8_SINT;

	case VK_FORMAT_B8G8R8_SRGB:
		return TEX_FORMAT_B8G8R8_SRGB;

	case VK_FORMAT_R8G8B8A8_UNORM:
		return TEX_FORMAT_R8G8B8A8_UNORM;

	case VK_FORMAT_R8G8B8A8_SNORM:
		return TEX_FORMAT_R8G8B8A8_SNORM;

	case VK_FORMAT_R8G8B8A8_USCALED:
		return TEX_FORMAT_R8G8B8A8_USCALED;

	case VK_FORMAT_R8G8B8A8_SSCALED:
		return TEX_FORMAT_R8G8B8A8_SSCALED;

	case VK_FORMAT_R8G8B8A8_UINT:
		return TEX_FORMAT_R8G8B8A8_UINT;

	case VK_FORMAT_R8G8B8A8_SINT:
		return TEX_FORMAT_R8G8B8A8_SINT;

	case VK_FORMAT_R8G8B8A8_SRGB:
		return TEX_FORMAT_R8G8B8A8_SRGB;

	case VK_FORMAT_B8G8R8A8_UNORM:
		return TEX_FORMAT_B8G8R8A8_UNORM;

	case VK_FORMAT_B8G8R8A8_SNORM:
		return TEX_FORMAT_B8G8R8A8_SNORM;

	case VK_FORMAT_B8G8R8A8_USCALED:
		return TEX_FORMAT_B8G8R8A8_USCALED;

	case VK_FORMAT_B8G8R8A8_SSCALED:
		return TEX_FORMAT_B8G8R8A8_SSCALED;

	case VK_FORMAT_B8G8R8A8_UINT:
		return TEX_FORMAT_B8G8R8A8_UINT;

	case VK_FORMAT_B8G8R8A8_SINT:
		return TEX_FORMAT_B8G8R8A8_SINT;

	case VK_FORMAT_B8G8R8A8_SRGB:
		return TEX_FORMAT_B8G8R8A8_SRGB;

	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		return TEX_FORMAT_A8B8G8R8_UNORM_PACK32;

	case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		return TEX_FORMAT_A8B8G8R8_SNORM_PACK32;

	case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
		return TEX_FORMAT_A8B8G8R8_USCALED_PACK32;

	case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
		return TEX_FORMAT_A8B8G8R8_SSCALED_PACK32;

	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		return TEX_FORMAT_A8B8G8R8_UINT_PACK32;

	case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		return TEX_FORMAT_A8B8G8R8_SINT_PACK32;

	case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		return TEX_FORMAT_A8B8G8R8_SRGB_PACK32;

	case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		return TEX_FORMAT_A2R10G10B10_UNORM_PACK32;

	case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
		return TEX_FORMAT_A2R10G10B10_SNORM_PACK32;

	case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
		return TEX_FORMAT_A2R10G10B10_USCALED_PACK32;

	case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
		return TEX_FORMAT_A2R10G10B10_SSCALED_PACK32;

	case VK_FORMAT_A2R10G10B10_UINT_PACK32:
		return TEX_FORMAT_A2R10G10B10_UINT_PACK32;

	case VK_FORMAT_A2R10G10B10_SINT_PACK32:
		return TEX_FORMAT_A2R10G10B10_SINT_PACK32;

	case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		return TEX_FORMAT_A2B10G10R10_UNORM_PACK32;

	case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
		return TEX_FORMAT_A2B10G10R10_SNORM_PACK32;

	case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
		return TEX_FORMAT_A2B10G10R10_USCALED_PACK32;

	case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
		return TEX_FORMAT_A2B10G10R10_SSCALED_PACK32;

	case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		return TEX_FORMAT_A2B10G10R10_UINT_PACK32;

	case VK_FORMAT_A2B10G10R10_SINT_PACK32:
		return TEX_FORMAT_A2B10G10R10_SINT_PACK32;

	case VK_FORMAT_R16_UNORM:
		return TEX_FORMAT_R16_UNORM;

	case VK_FORMAT_R16_SNORM:
		return TEX_FORMAT_R16_SNORM;

	case VK_FORMAT_R16_USCALED:
		return TEX_FORMAT_R16_USCALED;

	case VK_FORMAT_R16_SSCALED:
		return TEX_FORMAT_R16_SSCALED;

	case VK_FORMAT_R16_UINT:
		return TEX_FORMAT_R16_UINT;

	case VK_FORMAT_R16_SINT:
		return TEX_FORMAT_R16_SINT;

	case VK_FORMAT_R16_SFLOAT:
		return TEX_FORMAT_R16_SFLOAT;

	case VK_FORMAT_R16G16_UNORM:
		return TEX_FORMAT_R16G16_UNORM;

	case VK_FORMAT_R16G16_SNORM:
		return TEX_FORMAT_R16G16_SNORM;

	case VK_FORMAT_R16G16_USCALED:
		return TEX_FORMAT_R16G16_USCALED;

	case VK_FORMAT_R16G16_SSCALED:
		return TEX_FORMAT_R16G16_SSCALED;

	case VK_FORMAT_R16G16_UINT:
		return TEX_FORMAT_R16G16_UINT;

	case VK_FORMAT_R16G16_SINT:
		return TEX_FORMAT_R16G16_SINT;

	case VK_FORMAT_R16G16_SFLOAT:
		return TEX_FORMAT_R16G16_SFLOAT;

	case VK_FORMAT_R16G16B16_UNORM:
		return TEX_FORMAT_R16G16B16_UNORM;

	case VK_FORMAT_R16G16B16_SNORM:
		return TEX_FORMAT_R16G16B16_SNORM;

	case VK_FORMAT_R16G16B16_USCALED:
		return TEX_FORMAT_R16G16B16_USCALED;

	case VK_FORMAT_R16G16B16_SSCALED:
		return TEX_FORMAT_R16G16B16_SSCALED;

	case VK_FORMAT_R16G16B16_UINT:
		return TEX_FORMAT_R16G16B16_UINT;

	case VK_FORMAT_R16G16B16_SINT:
		return TEX_FORMAT_R16G16B16_SINT;

	case VK_FORMAT_R16G16B16_SFLOAT:
		return TEX_FORMAT_R16G16B16_SFLOAT;

	case VK_FORMAT_R16G16B16A16_UNORM:
		return TEX_FORMAT_R16G16B16A16_UNORM;

	case VK_FORMAT_R16G16B16A16_SNORM:
		return TEX_FORMAT_R16G16B16A16_SNORM;

	case VK_FORMAT_R16G16B16A16_USCALED:
		return TEX_FORMAT_R16G16B16A16_USCALED;

	case VK_FORMAT_R16G16B16A16_SSCALED:
		return TEX_FORMAT_R16G16B16A16_SSCALED;

	case VK_FORMAT_R16G16B16A16_UINT:
		return TEX_FORMAT_R16G16B16A16_UINT;

	case VK_FORMAT_R16G16B16A16_SINT:
		return TEX_FORMAT_R16G16B16A16_SINT;

	case VK_FORMAT_R16G16B16A16_SFLOAT:
		return TEX_FORMAT_R16G16B16A16_SFLOAT;

	case VK_FORMAT_R32_UINT:
		return TEX_FORMAT_R32_UINT;

	case VK_FORMAT_R32_SINT:
		return TEX_FORMAT_R32_SINT;

	case VK_FORMAT_R32_SFLOAT:
		return TEX_FORMAT_R32_SFLOAT;

	case VK_FORMAT_R32G32_UINT:
		return TEX_FORMAT_R32G32_UINT;

	case VK_FORMAT_R32G32_SINT:
		return TEX_FORMAT_R32G32_SINT;

	case VK_FORMAT_R32G32_SFLOAT:
		return TEX_FORMAT_R32G32_SFLOAT;

	case VK_FORMAT_R32G32B32_UINT:
		return TEX_FORMAT_R32G32B32_UINT;

	case VK_FORMAT_R32G32B32_SINT:
		return TEX_FORMAT_R32G32B32_SINT;

	case VK_FORMAT_R32G32B32_SFLOAT:
		return TEX_FORMAT_R32G32B32_SFLOAT;

	case VK_FORMAT_R32G32B32A32_UINT:
		return TEX_FORMAT_R32G32B32A32_UINT;

	case VK_FORMAT_R32G32B32A32_SINT:
		return TEX_FORMAT_R32G32B32A32_SINT;

	case VK_FORMAT_R32G32B32A32_SFLOAT:
		return TEX_FORMAT_R32G32B32A32_SFLOAT;

	case VK_FORMAT_R64_UINT:
		return TEX_FORMAT_R64_UINT;

	case VK_FORMAT_R64_SINT:
		return TEX_FORMAT_R64_SINT;

	case VK_FORMAT_R64_SFLOAT:
		return TEX_FORMAT_R64_SFLOAT;

	case VK_FORMAT_R64G64_UINT:
		return TEX_FORMAT_R64G64_UINT;

	case VK_FORMAT_R64G64_SINT:
		return TEX_FORMAT_R64G64_SINT;

	case VK_FORMAT_R64G64_SFLOAT:
		return TEX_FORMAT_R64G64_SFLOAT;

	case VK_FORMAT_R64G64B64_UINT:
		return TEX_FORMAT_R64G64B64_UINT;

	case VK_FORMAT_R64G64B64_SINT:
		return TEX_FORMAT_R64G64B64_SINT;

	case VK_FORMAT_R64G64B64_SFLOAT:
		return TEX_FORMAT_R64G64B64_SFLOAT;

	case VK_FORMAT_R64G64B64A64_UINT:
		return TEX_FORMAT_R64G64B64A64_UINT;

	case VK_FORMAT_R64G64B64A64_SINT:
		return TEX_FORMAT_R64G64B64A64_SINT;

	case VK_FORMAT_R64G64B64A64_SFLOAT:
		return TEX_FORMAT_R64G64B64A64_SFLOAT;

	case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
		return TEX_FORMAT_B10G11R11_UFLOAT_PACK32;

	case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
		return TEX_FORMAT_E5B9G9R9_UFLOAT_PACK32;

	case VK_FORMAT_D16_UNORM:
		return TEX_FORMAT_D16_UNORM;

	case VK_FORMAT_X8_D24_UNORM_PACK32:
		return TEX_FORMAT_X8_D24_UNORM_PACK32;

	case VK_FORMAT_D32_SFLOAT:
		return TEX_FORMAT_D32_SFLOAT;

	case VK_FORMAT_S8_UINT:
		return TEX_FORMAT_S8_UINT;

	case VK_FORMAT_D32_SFLOAT_S8_UINT:
		return TEX_FORMAT_D32_SFLOAT_S8_UINT;

	case VK_FORMAT_D24_UNORM_S8_UINT:
		return TEX_FORMAT_D24_UNORM_S8_UINT;

	default:
		dev::LogMgr::get_singleton()->print("[VULKAN:UTIL] Failed to find TextureFormat given VkFormat: %d", fmt);
		return TEX_FORMAT_MAX_ENUM;
	}
}

VkFormat vkutil::get_vk_texture_format(TextureFormat fmt)
{
	switch (fmt)
	{
	case TEX_FORMAT_R4G4_UNORM_PACK8:
		return VK_FORMAT_R4G4_UNORM_PACK8;

	case TEX_FORMAT_R4G4B4A4_UNORM_PACK16:
		return VK_FORMAT_R4G4B4A4_UNORM_PACK16;

	case TEX_FORMAT_B4G4R4A4_UNORM_PACK16:
		return VK_FORMAT_B4G4R4A4_UNORM_PACK16;

	case TEX_FORMAT_R5G6B5_UNORM_PACK16:
		return VK_FORMAT_R5G6B5_UNORM_PACK16;

	case TEX_FORMAT_B5G6R5_UNORM_PACK16:
		return VK_FORMAT_B5G6R5_UNORM_PACK16;

	case TEX_FORMAT_R5G5B5A1_UNORM_PACK16:
		return VK_FORMAT_R5G5B5A1_UNORM_PACK16;

	case TEX_FORMAT_B5G5R5A1_UNORM_PACK16:
		return VK_FORMAT_B5G5R5A1_UNORM_PACK16;

	case TEX_FORMAT_A1R5G5B5_UNORM_PACK16:
		return VK_FORMAT_A1R5G5B5_UNORM_PACK16;

	case TEX_FORMAT_R8_UNORM:
		return VK_FORMAT_R8_UNORM;

	case TEX_FORMAT_R8_SNORM:
		return VK_FORMAT_R8_SNORM;

	case TEX_FORMAT_R8_USCALED:
		return VK_FORMAT_R8_USCALED;

	case TEX_FORMAT_R8_SSCALED:
		return VK_FORMAT_R8_SSCALED;

	case TEX_FORMAT_R8_UINT:
		return VK_FORMAT_R8_UINT;

	case TEX_FORMAT_R8_SINT:
		return VK_FORMAT_R8_SINT;

	case TEX_FORMAT_R8_SRGB:
		return VK_FORMAT_R8_SRGB;

	case TEX_FORMAT_R8G8_UNORM:
		return VK_FORMAT_R8G8_UNORM;

	case TEX_FORMAT_R8G8_SNORM:
		return VK_FORMAT_R8G8_SNORM;

	case TEX_FORMAT_R8G8_USCALED:
		return VK_FORMAT_R8G8_USCALED;

	case TEX_FORMAT_R8G8_SSCALED:
		return VK_FORMAT_R8G8_SSCALED;

	case TEX_FORMAT_R8G8_UINT:
		return VK_FORMAT_R8G8_UINT;

	case TEX_FORMAT_R8G8_SINT:
		return VK_FORMAT_R8G8_SINT;

	case TEX_FORMAT_R8G8_SRGB:
		return VK_FORMAT_R8G8_SRGB;

	case TEX_FORMAT_R8G8B8_UNORM:
		return VK_FORMAT_R8G8B8_UNORM;

	case TEX_FORMAT_R8G8B8_SNORM:
		return VK_FORMAT_R8G8B8_SNORM;

	case TEX_FORMAT_R8G8B8_USCALED:
		return VK_FORMAT_R8G8B8_USCALED;

	case TEX_FORMAT_R8G8B8_SSCALED:
		return VK_FORMAT_R8G8B8_SSCALED;

	case TEX_FORMAT_R8G8B8_UINT:
		return VK_FORMAT_R8G8B8_UINT;

	case TEX_FORMAT_R8G8B8_SINT:
		return VK_FORMAT_R8G8B8_SINT;

	case TEX_FORMAT_R8G8B8_SRGB:
		return VK_FORMAT_R8G8B8_SRGB;

	case TEX_FORMAT_B8G8R8_UNORM:
		return VK_FORMAT_B8G8R8_UNORM;

	case TEX_FORMAT_B8G8R8_SNORM:
		return VK_FORMAT_B8G8R8_SNORM;

	case TEX_FORMAT_B8G8R8_USCALED:
		return VK_FORMAT_B8G8R8_USCALED;

	case TEX_FORMAT_B8G8R8_SSCALED:
		return VK_FORMAT_B8G8R8_SSCALED;

	case TEX_FORMAT_B8G8R8_UINT:
		return VK_FORMAT_B8G8R8_UINT;

	case TEX_FORMAT_B8G8R8_SINT:
		return VK_FORMAT_B8G8R8_SINT;

	case TEX_FORMAT_B8G8R8_SRGB:
		return VK_FORMAT_B8G8R8_SRGB;

	case TEX_FORMAT_R8G8B8A8_UNORM:
		return VK_FORMAT_R8G8B8A8_UNORM;

	case TEX_FORMAT_R8G8B8A8_SNORM:
		return VK_FORMAT_R8G8B8A8_SNORM;

	case TEX_FORMAT_R8G8B8A8_USCALED:
		return VK_FORMAT_R8G8B8A8_USCALED;

	case TEX_FORMAT_R8G8B8A8_SSCALED:
		return VK_FORMAT_R8G8B8A8_SSCALED;

	case TEX_FORMAT_R8G8B8A8_UINT:
		return VK_FORMAT_R8G8B8A8_UINT;

	case TEX_FORMAT_R8G8B8A8_SINT:
		return VK_FORMAT_R8G8B8A8_SINT;

	case TEX_FORMAT_R8G8B8A8_SRGB:
		return VK_FORMAT_R8G8B8A8_SRGB;

	case TEX_FORMAT_B8G8R8A8_UNORM:
		return VK_FORMAT_B8G8R8A8_UNORM;

	case TEX_FORMAT_B8G8R8A8_SNORM:
		return VK_FORMAT_B8G8R8A8_SNORM;

	case TEX_FORMAT_B8G8R8A8_USCALED:
		return VK_FORMAT_B8G8R8A8_USCALED;

	case TEX_FORMAT_B8G8R8A8_SSCALED:
		return VK_FORMAT_B8G8R8A8_SSCALED;

	case TEX_FORMAT_B8G8R8A8_UINT:
		return VK_FORMAT_B8G8R8A8_UINT;

	case TEX_FORMAT_B8G8R8A8_SINT:
		return VK_FORMAT_B8G8R8A8_SINT;

	case TEX_FORMAT_B8G8R8A8_SRGB:
		return VK_FORMAT_B8G8R8A8_SRGB;

	case TEX_FORMAT_A8B8G8R8_UNORM_PACK32:
		return VK_FORMAT_A8B8G8R8_UNORM_PACK32;

	case TEX_FORMAT_A8B8G8R8_SNORM_PACK32:
		return VK_FORMAT_A8B8G8R8_SNORM_PACK32;

	case TEX_FORMAT_A8B8G8R8_USCALED_PACK32:
		return VK_FORMAT_A8B8G8R8_USCALED_PACK32;

	case TEX_FORMAT_A8B8G8R8_SSCALED_PACK32:
		return VK_FORMAT_A8B8G8R8_SSCALED_PACK32;

	case TEX_FORMAT_A8B8G8R8_UINT_PACK32:
		return VK_FORMAT_A8B8G8R8_UINT_PACK32;

	case TEX_FORMAT_A8B8G8R8_SINT_PACK32:
		return VK_FORMAT_A8B8G8R8_SINT_PACK32;

	case TEX_FORMAT_A8B8G8R8_SRGB_PACK32:
		return VK_FORMAT_A8B8G8R8_SRGB_PACK32;

	case TEX_FORMAT_A2R10G10B10_UNORM_PACK32:
		return VK_FORMAT_A2R10G10B10_UNORM_PACK32;

	case TEX_FORMAT_A2R10G10B10_SNORM_PACK32:
		return VK_FORMAT_A2R10G10B10_SNORM_PACK32;

	case TEX_FORMAT_A2R10G10B10_USCALED_PACK32:
		return VK_FORMAT_A2R10G10B10_USCALED_PACK32;

	case TEX_FORMAT_A2R10G10B10_SSCALED_PACK32:
		return VK_FORMAT_A2R10G10B10_SSCALED_PACK32;

	case TEX_FORMAT_A2R10G10B10_UINT_PACK32:
		return VK_FORMAT_A2R10G10B10_UINT_PACK32;

	case TEX_FORMAT_A2R10G10B10_SINT_PACK32:
		return VK_FORMAT_A2R10G10B10_SINT_PACK32;

	case TEX_FORMAT_A2B10G10R10_UNORM_PACK32:
		return VK_FORMAT_A2B10G10R10_UNORM_PACK32;

	case TEX_FORMAT_A2B10G10R10_SNORM_PACK32:
		return VK_FORMAT_A2B10G10R10_SNORM_PACK32;

	case TEX_FORMAT_A2B10G10R10_USCALED_PACK32:
		return VK_FORMAT_A2B10G10R10_USCALED_PACK32;

	case TEX_FORMAT_A2B10G10R10_SSCALED_PACK32:
		return VK_FORMAT_A2B10G10R10_SSCALED_PACK32;

	case TEX_FORMAT_A2B10G10R10_UINT_PACK32:
		return VK_FORMAT_A2B10G10R10_UINT_PACK32;

	case TEX_FORMAT_A2B10G10R10_SINT_PACK32:
		return VK_FORMAT_A2B10G10R10_SINT_PACK32;

	case TEX_FORMAT_R16_UNORM:
		return VK_FORMAT_R16_UNORM;

	case TEX_FORMAT_R16_SNORM:
		return VK_FORMAT_R16_SNORM;

	case TEX_FORMAT_R16_USCALED:
		return VK_FORMAT_R16_USCALED;

	case TEX_FORMAT_R16_SSCALED:
		return VK_FORMAT_R16_SSCALED;

	case TEX_FORMAT_R16_UINT:
		return VK_FORMAT_R16_UINT;

	case TEX_FORMAT_R16_SINT:
		return VK_FORMAT_R16_SINT;

	case TEX_FORMAT_R16_SFLOAT:
		return VK_FORMAT_R16_SFLOAT;

	case TEX_FORMAT_R16G16_UNORM:
		return VK_FORMAT_R16G16_UNORM;

	case TEX_FORMAT_R16G16_SNORM:
		return VK_FORMAT_R16G16_SNORM;

	case TEX_FORMAT_R16G16_USCALED:
		return VK_FORMAT_R16G16_USCALED;

	case TEX_FORMAT_R16G16_SSCALED:
		return VK_FORMAT_R16G16_SSCALED;

	case TEX_FORMAT_R16G16_UINT:
		return VK_FORMAT_R16G16_UINT;

	case TEX_FORMAT_R16G16_SINT:
		return VK_FORMAT_R16G16_SINT;

	case TEX_FORMAT_R16G16_SFLOAT:
		return VK_FORMAT_R16G16_SFLOAT;

	case TEX_FORMAT_R16G16B16_UNORM:
		return VK_FORMAT_R16G16B16_UNORM;

	case TEX_FORMAT_R16G16B16_SNORM:
		return VK_FORMAT_R16G16B16_SNORM;

	case TEX_FORMAT_R16G16B16_USCALED:
		return VK_FORMAT_R16G16B16_USCALED;

	case TEX_FORMAT_R16G16B16_SSCALED:
		return VK_FORMAT_R16G16B16_SSCALED;

	case TEX_FORMAT_R16G16B16_UINT:
		return VK_FORMAT_R16G16B16_UINT;

	case TEX_FORMAT_R16G16B16_SINT:
		return VK_FORMAT_R16G16B16_SINT;

	case TEX_FORMAT_R16G16B16_SFLOAT:
		return VK_FORMAT_R16G16B16_SFLOAT;

	case TEX_FORMAT_R16G16B16A16_UNORM:
		return VK_FORMAT_R16G16B16A16_UNORM;

	case TEX_FORMAT_R16G16B16A16_SNORM:
		return VK_FORMAT_R16G16B16A16_SNORM;

	case TEX_FORMAT_R16G16B16A16_USCALED:
		return VK_FORMAT_R16G16B16A16_USCALED;

	case TEX_FORMAT_R16G16B16A16_SSCALED:
		return VK_FORMAT_R16G16B16A16_SSCALED;

	case TEX_FORMAT_R16G16B16A16_UINT:
		return VK_FORMAT_R16G16B16A16_UINT;

	case TEX_FORMAT_R16G16B16A16_SINT:
		return VK_FORMAT_R16G16B16A16_SINT;

	case TEX_FORMAT_R16G16B16A16_SFLOAT:
		return VK_FORMAT_R16G16B16A16_SFLOAT;

	case TEX_FORMAT_R32_UINT:
		return VK_FORMAT_R32_UINT;

	case TEX_FORMAT_R32_SINT:
		return VK_FORMAT_R32_SINT;

	case TEX_FORMAT_R32_SFLOAT:
		return VK_FORMAT_R32_SFLOAT;

	case TEX_FORMAT_R32G32_UINT:
		return VK_FORMAT_R32G32_UINT;

	case TEX_FORMAT_R32G32_SINT:
		return VK_FORMAT_R32G32_SINT;

	case TEX_FORMAT_R32G32_SFLOAT:
		return VK_FORMAT_R32G32_SFLOAT;

	case TEX_FORMAT_R32G32B32_UINT:
		return VK_FORMAT_R32G32B32_UINT;

	case TEX_FORMAT_R32G32B32_SINT:
		return VK_FORMAT_R32G32B32_SINT;

	case TEX_FORMAT_R32G32B32_SFLOAT:
		return VK_FORMAT_R32G32B32_SFLOAT;

	case TEX_FORMAT_R32G32B32A32_UINT:
		return VK_FORMAT_R32G32B32A32_UINT;

	case TEX_FORMAT_R32G32B32A32_SINT:
		return VK_FORMAT_R32G32B32A32_SINT;

	case TEX_FORMAT_R32G32B32A32_SFLOAT:
		return VK_FORMAT_R32G32B32A32_SFLOAT;

	case TEX_FORMAT_R64_UINT:
		return VK_FORMAT_R64_UINT;

	case TEX_FORMAT_R64_SINT:
		return VK_FORMAT_R64_SINT;

	case TEX_FORMAT_R64_SFLOAT:
		return VK_FORMAT_R64_SFLOAT;

	case TEX_FORMAT_R64G64_UINT:
		return VK_FORMAT_R64G64_UINT;

	case TEX_FORMAT_R64G64_SINT:
		return VK_FORMAT_R64G64_SINT;

	case TEX_FORMAT_R64G64_SFLOAT:
		return VK_FORMAT_R64G64_SFLOAT;

	case TEX_FORMAT_R64G64B64_UINT:
		return VK_FORMAT_R64G64B64_UINT;

	case TEX_FORMAT_R64G64B64_SINT:
		return VK_FORMAT_R64G64B64_SINT;

	case TEX_FORMAT_R64G64B64_SFLOAT:
		return VK_FORMAT_R64G64B64_SFLOAT;

	case TEX_FORMAT_R64G64B64A64_UINT:
		return VK_FORMAT_R64G64B64A64_UINT;

	case TEX_FORMAT_R64G64B64A64_SINT:
		return VK_FORMAT_R64G64B64A64_SINT;

	case TEX_FORMAT_R64G64B64A64_SFLOAT:
		return VK_FORMAT_R64G64B64A64_SFLOAT;

	case TEX_FORMAT_B10G11R11_UFLOAT_PACK32:
		return VK_FORMAT_B10G11R11_UFLOAT_PACK32;

	case TEX_FORMAT_E5B9G9R9_UFLOAT_PACK32:
		return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;

	case TEX_FORMAT_D16_UNORM:
		return VK_FORMAT_D16_UNORM;

	case TEX_FORMAT_X8_D24_UNORM_PACK32:
		return VK_FORMAT_X8_D24_UNORM_PACK32;

	case TEX_FORMAT_D32_SFLOAT:
		return VK_FORMAT_D32_SFLOAT;

	case TEX_FORMAT_S8_UINT:
		return VK_FORMAT_S8_UINT;

	case TEX_FORMAT_D32_SFLOAT_S8_UINT:
		return VK_FORMAT_D32_SFLOAT_S8_UINT;

	case TEX_FORMAT_D24_UNORM_S8_UINT:
		return VK_FORMAT_D24_UNORM_S8_UINT;

	default:
		dev::LogMgr::get_singleton()->print("[VULKAN:UTIL] Failed to find VkFormat given TextureFormat: %d", fmt);
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
		dev::LogMgr::get_singleton()->print("[VULKAN:UTIL] Failed to find VkImageTiling given TextureTiling: %d", tiling);
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
		dev::LogMgr::get_singleton()->print("[VULKAN:UTIL] Failed to find VkFilter given TextureFilter: %d", filter);
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
		dev::LogMgr::get_singleton()->print("[VULKAN:UTIL] Failed to find VkSamplerAddressMode given TextureWrap: %d", wrap);
		return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
	}
}

VkShaderStageFlagBits vkutil::get_vk_shader_flag_bits(ShaderProgramType type)
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
		dev::LogMgr::get_singleton()->print("[VULKAN:UTIL] Failed to find VkShaderStageFlagBits given ShaderType: %d", type);
		return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	}
}

VkImageViewType vkutil::get_vk_image_view_type(TextureType type)
{
	switch (type)
	{
	case TEX_TYPE_1D:
		return VK_IMAGE_VIEW_TYPE_1D;

	case TEX_TYPE_1D_ARRAY:
		return VK_IMAGE_VIEW_TYPE_1D_ARRAY;

	case TEX_TYPE_2D:
		return VK_IMAGE_VIEW_TYPE_2D;

	case TEX_TYPE_2D_ARRAY:
		return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

	case TEX_TYPE_3D:
		return VK_IMAGE_VIEW_TYPE_3D;

	case TEX_TYPE_CUBE:
		return VK_IMAGE_VIEW_TYPE_CUBE;

	case TEX_TYPE_CUBE_ARRAY:
		return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;

	default:
		dev::LogMgr::get_singleton()->print("[VULKAN:UTIL] Failed to find VkImageViewType given TextureType: %d", type);
		return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
	}
}

VkImageType vkutil::get_vk_image_type(TextureType type)
{
	switch (type)
	{
	case TEX_TYPE_1D:
		return VK_IMAGE_TYPE_1D;

	case TEX_TYPE_1D_ARRAY:
		return VK_IMAGE_TYPE_1D;

	case TEX_TYPE_2D:
		return VK_IMAGE_TYPE_2D;

	case TEX_TYPE_2D_ARRAY:
		return VK_IMAGE_TYPE_2D;

	case TEX_TYPE_3D:
		return VK_IMAGE_TYPE_3D;

	case TEX_TYPE_CUBE:
		return VK_IMAGE_TYPE_2D;

	case TEX_TYPE_CUBE_ARRAY:
		return VK_IMAGE_TYPE_2D;

	default:
		dev::LogMgr::get_singleton()->print("[VULKAN:UTIL] Failed to find VkImageType given TextureType: %d", type);
		return VK_IMAGE_TYPE_MAX_ENUM;
	}
}

VkBlendOp vkutil::get_vk_blend_op(BlendOp op)
{
	switch (op)
	{
	case BLEND_OP_ADD:
		return VK_BLEND_OP_ADD;

	case BLEND_OP_SUBTRACT:
		return VK_BLEND_OP_SUBTRACT;

	case BLEND_OP_REVERSE_SUBTRACT:
		return VK_BLEND_OP_REVERSE_SUBTRACT;

	case BLEND_OP_MIN:
		return VK_BLEND_OP_MIN;

	case BLEND_OP_MAX:
		return VK_BLEND_OP_MAX;

	case BLEND_OP_ZERO:
		return VK_BLEND_OP_ZERO_EXT;

	case BLEND_OP_SRC:
		return VK_BLEND_OP_SRC_EXT;

	case BLEND_OP_DST:
		return VK_BLEND_OP_DST_EXT;

	case BLEND_OP_SRC_OVER:
		return VK_BLEND_OP_SRC_OVER_EXT;

	case BLEND_OP_DST_OVER:
		return VK_BLEND_OP_DST_OVER_EXT;

	case BLEND_OP_SRC_IN:
		return VK_BLEND_OP_SRC_IN_EXT;

	case BLEND_OP_DST_IN:
		return VK_BLEND_OP_DST_IN_EXT;

	case BLEND_OP_SRC_OUT:
		return VK_BLEND_OP_SRC_OUT_EXT;

	case BLEND_OP_DST_OUT:
		return VK_BLEND_OP_DST_OUT_EXT;

	case BLEND_OP_SRC_ATOP:
		return VK_BLEND_OP_SRC_ATOP_EXT;

	case BLEND_OP_DST_ATOP:
		return VK_BLEND_OP_DST_ATOP_EXT;

	case BLEND_OP_XOR:
		return VK_BLEND_OP_XOR_EXT;

	case BLEND_OP_MULTIPLY:
		return VK_BLEND_OP_MULTIPLY_EXT;

	case BLEND_OP_SCREEN:
		return VK_BLEND_OP_SCREEN_EXT;

	case BLEND_OP_OVERLAY:
		return VK_BLEND_OP_OVERLAY_EXT;

	case BLEND_OP_DARKEN:
		return VK_BLEND_OP_DARKEN_EXT;

	case BLEND_OP_LIGHTEN:
		return VK_BLEND_OP_LIGHTEN_EXT;

	case BLEND_OP_COLOUR_DODGE:
		return VK_BLEND_OP_COLORDODGE_EXT;

	case BLEND_OP_COLOUR_BURN:
		return VK_BLEND_OP_COLORBURN_EXT;

	case BLEND_OP_HARDLIGHT:
		return VK_BLEND_OP_HARDLIGHT_EXT;

	case BLEND_OP_SOFTLIGHT:
		return VK_BLEND_OP_SOFTLIGHT_EXT;

	case BLEND_OP_DIFFERENCE:
		return VK_BLEND_OP_DIFFERENCE_EXT;

	case BLEND_OP_EXCLUSION:
		return VK_BLEND_OP_EXCLUSION_EXT;

	case BLEND_OP_INVERT:
		return VK_BLEND_OP_INVERT_EXT;

	case BLEND_OP_INVERT_RGB:
		return VK_BLEND_OP_INVERT_RGB_EXT;

	case BLEND_OP_LINEAR_DODGE:
		return VK_BLEND_OP_LINEARDODGE_EXT;

	case BLEND_OP_LINEAR_BURN:
		return VK_BLEND_OP_LINEARBURN_EXT;

	case BLEND_OP_VIVID_LIGHT:
		return VK_BLEND_OP_VIVIDLIGHT_EXT;

	case BLEND_OP_LINEAR_LIGHT:
		return VK_BLEND_OP_LINEARLIGHT_EXT;

	case BLEND_OP_PIN_LIGHT:
		return VK_BLEND_OP_PINLIGHT_EXT;

	case BLEND_OP_HARD_MIX:
		return VK_BLEND_OP_HARDMIX_EXT;

	case BLEND_OP_HSL_HUE:
		return VK_BLEND_OP_HSL_HUE_EXT;

	case BLEND_OP_HSL_SATURATION:
		return VK_BLEND_OP_HSL_SATURATION_EXT;

	case BLEND_OP_HSL_COLOUR:
		return VK_BLEND_OP_HSL_COLOR_EXT;

	case BLEND_OP_HSL_LUMINOSITY:
		return VK_BLEND_OP_HSL_LUMINOSITY_EXT;

	case BLEND_OP_PLUS:
		return VK_BLEND_OP_PLUS_EXT;

	case BLEND_OP_PLUS_CLAMPED:
		return VK_BLEND_OP_PLUS_CLAMPED_EXT;

	case BLEND_OP_PLUS_CLAMPED_ALPHA:
		return VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT;

	case BLEND_OP_PLUS_DARKER:
		return VK_BLEND_OP_PLUS_DARKER_EXT;

	case BLEND_OP_MINUS:
		return VK_BLEND_OP_MINUS_EXT;

	case BLEND_OP_MINUS_CLAMPED:
		return VK_BLEND_OP_MINUS_CLAMPED_EXT;

	case BLEND_OP_CONTRAST:
		return VK_BLEND_OP_CONTRAST_EXT;

	case BLEND_OP_INVERT_OVG:
		return VK_BLEND_OP_INVERT_OVG_EXT;

	case BLEND_OP_RED:
		return VK_BLEND_OP_RED_EXT;

	case BLEND_OP_GREEN:
		return VK_BLEND_OP_GREEN_EXT;

	case BLEND_OP_BLUE:
		return VK_BLEND_OP_BLUE_EXT;

	default:
		dev::LogMgr::get_singleton()->print("[VULKAN:UTIL] Failed to find VkBlendOp given BlendOp: %d", op);
		return VK_BLEND_OP_MAX_ENUM;
	}
}

VkBlendFactor vkutil::get_vk_blend_factor(BlendFactor factor)
{
	switch (factor)
	{
	case BLEND_FACTOR_ZERO:
		return VK_BLEND_FACTOR_ZERO;

	case BLEND_FACTOR_ONE:
		return VK_BLEND_FACTOR_ONE;

	case BLEND_FACTOR_SRC_COLOUR:
		return VK_BLEND_FACTOR_SRC_COLOR;

	case BLEND_FACTOR_ONE_MINUS_SRC_COLOUR:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;

	case BLEND_FACTOR_DST_COLOUR:
		return VK_BLEND_FACTOR_DST_COLOR;

	case BLEND_FACTOR_ONE_MINUS_DST_COLOUR:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

	case BLEND_FACTOR_SRC_ALPHA:
		return VK_BLEND_FACTOR_SRC_ALPHA;

	case BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

	case BLEND_FACTOR_DST_ALPHA:
		return VK_BLEND_FACTOR_DST_ALPHA;

	case BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

	case BLEND_FACTOR_CONSTANT_COLOUR:
		return VK_BLEND_FACTOR_CONSTANT_COLOR;

	case BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOUR:
		return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;

	case BLEND_FACTOR_CONSTANT_ALPHA:
		return VK_BLEND_FACTOR_CONSTANT_ALPHA;

	case BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
		return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;

	case BLEND_FACTOR_SRC_ALPHA_SATURATE:
		return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;

	case BLEND_FACTOR_SRC1_COLOUR:
		return VK_BLEND_FACTOR_SRC1_COLOR;

	case BLEND_FACTOR_ONE_MINUS_SRC1_COLOUR:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;

	case BLEND_FACTOR_SRC1_ALPHA:
		return VK_BLEND_FACTOR_SRC1_ALPHA;

	case BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;

	default:
		dev::LogMgr::get_singleton()->print("[VULKAN:UTIL] Failed to find VkBlendFactor given BlendFactor: %d", factor);
		return VK_BLEND_FACTOR_MAX_ENUM;
	}
}

VkCompareOp vkutil::get_vk_compare_op(CompareOp op)
{
	switch (op)
	{
	case COMPARE_OP_NEVER:
		return VK_COMPARE_OP_NEVER;

	case COMPARE_OP_LESS:
		return VK_COMPARE_OP_LESS;

	case COMPARE_OP_EQUAL:
		return VK_COMPARE_OP_EQUAL;

	case COMPARE_OP_LESS_OR_EQUAL:
		return VK_COMPARE_OP_LESS_OR_EQUAL;

	case COMPARE_OP_GREATER:
		return VK_COMPARE_OP_GREATER;

	case COMPARE_OP_NOT_EQUAL:
		return VK_COMPARE_OP_NOT_EQUAL;

	case COMPARE_OP_GREATER_OR_EQUAL:
		return VK_COMPARE_OP_GREATER_OR_EQUAL;

	case COMPARE_OP_ALWAYS:
		return VK_COMPARE_OP_ALWAYS;

	default:
		dev::LogMgr::get_singleton()->print("[VULKAN:UTIL] Failed to find VkCompareOp given CompareOp: %d", op);
		return VK_COMPARE_OP_MAX_ENUM;
	}
}

VkLogicOp vkutil::get_vk_logic_op(LogicOp op)
{
	switch (op)
	{
	case LOGIC_OP_CLEAR:
		return VK_LOGIC_OP_CLEAR;

	case LOGIC_OP_AND:
		return VK_LOGIC_OP_AND;

	case LOGIC_OP_AND_REVERSE:
		return VK_LOGIC_OP_AND_REVERSE;

	case LOGIC_OP_COPY:
		return VK_LOGIC_OP_COPY;

	case LOGIC_OP_AND_INVERTED:
		return VK_LOGIC_OP_AND_INVERTED;

	case LOGIC_OP_NO_OP:
		return VK_LOGIC_OP_NO_OP;

	case LOGIC_OP_XOR:
		return VK_LOGIC_OP_XOR;

	case LOGIC_OP_OR:
		return VK_LOGIC_OP_OR;

	case LOGIC_OP_NOR:
		return VK_LOGIC_OP_NOR;

	case LOGIC_OP_EQUIVALENT:
		return VK_LOGIC_OP_EQUIVALENT;

	case LOGIC_OP_INVERT:
		return VK_LOGIC_OP_INVERT;

	case LOGIC_OP_OR_REVERSE:
		return VK_LOGIC_OP_OR_REVERSE;

	case LOGIC_OP_COPY_INVERTED:
		return VK_LOGIC_OP_COPY_INVERTED;

	case LOGIC_OP_OR_INVERTED:
		return VK_LOGIC_OP_OR_INVERTED;

	case LOGIC_OP_NAND:
		return VK_LOGIC_OP_NAND;

	case LOGIC_OP_SET:
		return VK_LOGIC_OP_SET;

	default:
		dev::LogMgr::get_singleton()->print("[VULKAN:UTIL] Failed to find VkLogicOp given LogicOp: %d", op);
		return VK_LOGIC_OP_MAX_ENUM;
	}
}

VkBorderColor vkutil::get_vk_border_colour(TextureBorderColour border_colour)
{
	switch (border_colour)
	{
	case TEX_BORDER_COLOUR_FLOAT_TRANSPARENT_BLACK:
		return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

	case TEX_BORDER_COLOUR_INT_TRANSPARENT_BLACK:
		return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;

	case TEX_BORDER_COLOUR_FLOAT_OPAQUE_BLACK:
		return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

	case TEX_BORDER_COLOUR_INT_OPAQUE_BLACK:
		return VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	case TEX_BORDER_COLOUR_FLOAT_OPAQUE_WHITE:
		return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

	case TEX_BORDER_COLOUR_INT_OPAQUE_WHITE:
		return VK_BORDER_COLOR_INT_OPAQUE_WHITE;

	default:
		dev::LogMgr::get_singleton()->print("[VULKAN:UTIL] Failed to find VkBorderColor given TextureBorderColour: %d", border_colour);
		return VK_BORDER_COLOR_MAX_ENUM;
	}
}

VkCullModeFlagBits vkutil::get_vk_cull_mode(CullMode cull)
{
	switch (cull)
	{
	case CULL_MODE_BACK:
		return VK_CULL_MODE_BACK_BIT;

	case CULL_MODE_FRONT:
		return VK_CULL_MODE_FRONT_BIT;

	case CULL_MODE_FRONT_AND_BACK:
		return VK_CULL_MODE_FRONT_AND_BACK;

	default:
		dev::LogMgr::get_singleton()->print("[VULKAN:UTIL] Failed to find VkCullModeFlagBits given CullMode: %d", cull);
		return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
	}
}

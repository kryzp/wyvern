#ifndef VK_UTIL_H
#define VK_UTIL_H

#include <vulkan/vulkan.h>
#include <wvn/common.h>
#include <wvn/graphics/texture.h>
#include <wvn/graphics/shader.h>
#include <wvn/graphics/gpu_buffer.h>
#include <wvn/container/vector.h>

namespace wvn::gfx
{
	namespace vkutil
	{
		VkSurfaceFormatKHR choose_swap_surface_format(const Vector<VkSurfaceFormatKHR>& available_surface_formats);
		VkPresentModeKHR choose_swap_present_mode(const Vector<VkPresentModeKHR>& available_present_modes);
		VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

		u32 find_memory_type(VkPhysicalDevice physical_device, u32 filter, VkMemoryPropertyFlags properties);

		VkFormat find_supported_format(VkPhysicalDevice device, const Vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat find_depth_format(VkPhysicalDevice device);
		bool has_stencil_component(VkFormat format);

		VkFormat get_vk_texture_format(TextureFormat fmt);
		TextureFormat get_wvn_texture_format(VkFormat fmt);
		VkImageTiling get_vk_texture_tile(TextureTiling tiling);
		VkFilter get_vk_filter(TextureFilter filter);
		VkSamplerAddressMode get_vk_address_mode(TextureWrap wrap);
		VkShaderStageFlagBits get_vk_shader_flag_bits(ShaderType type);

		VkCommandBuffer begin_single_time_commands(VkCommandPool cmd_pool, VkDevice device);
		void end_single_time_commands(VkCommandPool cmd_pool, VkCommandBuffer cmd_buf, VkDevice device, VkQueue graphics);
	}
}

#endif // VK_UTIL_H

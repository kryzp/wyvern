#ifndef VK_UTIL_H
#define VK_UTIL_H

#include <vulkan/vulkan.h>
#include <wvn/util/types.h>
#include <wvn/graphics/texture.h>

namespace wvn::gfx
{
	namespace vkutil
	{
		u32 find_memory_type(VkPhysicalDevice physical_device, u32 filter, VkMemoryPropertyFlags properties);

		VkFormat get_vk_texture_format(TextureFormat fmt);
		VkImageTiling get_vk_texture_tile(TextureTiling tiling);
		VkFilter get_vk_filter(TextureFilter filter);
		VkSamplerAddressMode get_vk_address_mode(TextureWrap wrap);

		VkCommandBuffer begin_single_time_commands(VkDevice device, VkCommandPool cmd_pool);
		void end_single_time_commands(VkDevice device, VkCommandPool cmd_pool, VkCommandBuffer cmd_buf, VkQueue graphics_queue);
	}
}

#endif // VK_UTIL_H

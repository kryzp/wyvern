#pragma once

#include <vulkan/vulkan.h>

#include <wvn/util/types.h>

namespace wvn::gfx
{
	namespace vkutil
	{
		u32 find_memory_type(VkPhysicalDevice physical_device, u32 filter, VkMemoryPropertyFlags properties);
	}
}

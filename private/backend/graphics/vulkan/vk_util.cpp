#include <backend/graphics/vulkan/vk_util.h>
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

	WVN_ERROR("[VULKAN:DEBUG] Failed to find suitable memory type.");
	return 0;
}

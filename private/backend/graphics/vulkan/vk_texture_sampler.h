#ifndef VK_TEXTURE_SAMPLER_H
#define VK_TEXTURE_SAMPLER_H

#include <vulkan/vulkan.h>
#include <wvn/graphics/texture_sampler.h>

namespace wvn::gfx
{
	class VulkanTextureSampler : public TextureSampler
	{
	public:
		VulkanTextureSampler();
		VulkanTextureSampler(const Style& style);
		~VulkanTextureSampler();

		void clean_up();
		VkSampler bind(VkDevice device, VkPhysicalDeviceProperties properties, int mip_levels);
		VkSampler sampler() const;

	private:
		VkSampler m_sampler;
	};
}

#endif // VK_TEXTURE_SAMPLER_H

#ifndef VK_TEXTURE_SAMPLER_H
#define VK_TEXTURE_SAMPLER_H

#include <vulkan/vulkan.h>
#include <wvn/graphics/texture_sampler.h>

namespace wvn::gfx
{
	class VulkanTextureSampler
	{
	public:
		VulkanTextureSampler();
		~VulkanTextureSampler();

		void clean_up();

		void create(VkDevice device, VkPhysicalDeviceProperties properties, const TextureSampler& style);

		VkSampler sampler() const;

	private:
		VkSampler m_sampler;
	};
}

#endif // VK_TEXTURE_SAMPLER_H

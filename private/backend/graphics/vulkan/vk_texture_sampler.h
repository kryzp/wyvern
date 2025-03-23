#ifndef VK_TEXTURE_SAMPLER_H_
#define VK_TEXTURE_SAMPLER_H_

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

		void clean_up() override;

		VkSampler bind(VkDevice device, VkPhysicalDeviceProperties properties, int mip_levels);
		VkSampler sampler() const;

	private:
		VkSampler m_sampler;
	};
}

#endif // VK_TEXTURE_SAMPLER_H_

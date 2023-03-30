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
		VulkanTextureSampler(TextureFilter filter, TextureWrap wrap_x, TextureWrap wrap_y, TextureWrap wrap_z);
		~VulkanTextureSampler();

		void clean_up();

		void create(VkDevice device, VkPhysicalDeviceProperties properties);

		VkSampler sampler() const;

	private:
		VkSampler m_sampler;
	};
}

#endif // VK_TEXTURE_SAMPLER_H

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
		VulkanTextureSampler(const TextureSampler& style, VkDevice device, VkPhysicalDevice physical_device);
		~VulkanTextureSampler();

		void init(VkDevice device, VkPhysicalDevice physical_device);
		void clean_up();

		void create(const TextureSampler& style);

		VkSampler sampler() const;

	private:
		VkDevice m_device; // todo: (global for all classes, idk why im putting the todo here): make the access of m_device and m_physical_device somehow global? cleans stuff up.
		VkPhysicalDevice m_physical_device;
		VkSampler m_sampler;
	};
}

#endif // VK_TEXTURE_SAMPLER_H

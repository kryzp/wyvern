#ifndef VK_TEXTURE_H
#define VK_TEXTURE_H

#include <vulkan/vulkan.h>

#include <backend/graphics/vulkan/vk_image.h>
#include <backend/graphics/vulkan/vk_image_view.h>
#include <backend/graphics/vulkan/vk_buffer.h>
#include <backend/graphics/vulkan/vk_texture_sampler.h>

#include <wvn/graphics/texture.h>
#include <wvn/graphics/image.h>

#include <wvn/util/types.h>

namespace wvn::gfx
{
	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture();
		VulkanTexture(VkDevice device, VkPhysicalDevice physical_device);
		~VulkanTexture() override;

		void init(VkDevice device, VkPhysicalDevice physical_device);
		void clean_up();

		void create(const Image& image) override;
		void create(u32 width, u32 height, TextureFormat format, TextureTiling tiling) override;

		VulkanImage& image();
		const VulkanImage& image() const;

		VulkanImageView& view();
		const VulkanImageView& view() const;

		VulkanTextureSampler& sampler();
		const VulkanTextureSampler& sampler() const;

		int width() const;
		int height() const;

	private:
		VulkanImage m_image;
		VulkanImageView m_view;

		VulkanTextureSampler m_sampler;

		TextureFormat m_format;
		TextureTiling m_tiling;

		int m_width;
		int m_height;
	};
}

#endif // VK_TEXTURE_H

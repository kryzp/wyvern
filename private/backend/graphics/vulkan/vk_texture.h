#ifndef VK_TEXTURE_H
#define VK_TEXTURE_H

#include <vulkan/vulkan.h>

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
		~VulkanTexture() override;

		void init(VkDevice device, VkPhysicalDevice physical_device, VkPhysicalDeviceProperties properties);
		void clean_up();

		void create(const Image& image) override;
		void create(u32 width, u32 height, TextureFormat format, TextureTiling tiling) override;

		void transition_layout(VkFormat fmt, VkImageLayout new_layout, VkCommandPool cmd_pool, VkQueue graphics);

		VkImage image() const;
		VkImageView image_view() const;

		VulkanTextureSampler& sampler();
		const VulkanTextureSampler& sampler() const;

		TextureMetaData meta_data() const override;

		u32 width() const;
		u32 height() const;

	private:
		void create_internal_resources();
		VkImageView generate_view() const;

		VkDevice m_device;
		VkPhysicalDevice m_physical_device;
		VkPhysicalDeviceProperties m_properties;

		VkImage m_image;
		VkDeviceMemory m_image_memory;
		VkImageLayout m_image_layout;
		VkImageView m_view;

		VulkanTextureSampler m_sampler;

		TextureFormat m_format;
		TextureTiling m_tiling;

		u32 m_width;
		u32 m_height;
	};
}

#endif // VK_TEXTURE_H

#ifndef VK_TEXTURE_H
#define VK_TEXTURE_H

#include <vulkan/vulkan.h>

#include <backend/graphics/vulkan/vk_buffer.h>
#include <backend/graphics/vulkan/vk_texture_sampler.h>

#include <wvn/graphics/texture.h>
#include <wvn/graphics/image.h>

#include <wvn/common.h>

namespace wvn::gfx
{
	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(VulkanBackend* backend);
		~VulkanTexture() override;

		void clean_up();

		void create(const Image& image, TextureType type, u32 mip_levels, VkSampleCountFlagBits num_samples, bool transient);
		void create(u32 width, u32 height, TextureFormat format, TextureTiling tiling, TextureType type, u32 mip_levels, VkSampleCountFlagBits num_samples, bool transient);

		void transition_layout(VkImageLayout new_layout);
		void generate_mipmaps();

		u32 get_layer_count() const;
		u32 get_face_count() const;

		VkImage image() const;
		VkImageView image_view() const;

		u32 width() const override;
		u32 height() const override;
		u32 mip_levels() const;
		VkSampleCountFlagBits num_samples() const;
		bool transient() const;

		TextureMetaData meta_data() const override;

	private:
		void create_internal_resources();
		VkImageView generate_view() const;

		VulkanBackend* m_backend;

		VkImage m_image;
		VkDeviceMemory m_image_memory;
		VkImageLayout m_image_layout;
		VkImageView m_view;
		u32 m_mip_levels;
		VkSampleCountFlagBits m_num_samples;
		bool m_transient;

		TextureFormat m_format;
		TextureTiling m_tiling;
		TextureType m_type;

		u32 m_width;
		u32 m_height;
	};
}

#endif // VK_TEXTURE_H

#ifndef VK_TEXTURE_H_
#define VK_TEXTURE_H_

#include <vulkan/vulkan.h>

#include <backend/graphics/vulkan/vk_buffer.h>
#include <backend/graphics/vulkan/vk_texture_sampler.h>

#include <wvn/graphics/texture.h>
#include <wvn/graphics/image.h>

#include <wvn/common.h>

namespace wvn::gfx
{
	class RenderTarget;

	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(VulkanBackend* backend);
		~VulkanTexture() override;

		void clean_up() override;

		void from_image(const Image& image, TextureType type, u32 mip_levels, VkSampleCountFlagBits num_samples);

		void init_size(u32 width, u32 height);
		void init_metadata(TextureFormat format, TextureTiling tiling, TextureType type);
		void init_mip_levels(u32 mip_levels);
		void init_sample_count(VkSampleCountFlagBits num_samples);
		void init_transient(bool transient);

		void create_internal_resources();

		void transition_layout(VkImageLayout new_layout);
		void generate_mipmaps() const;

		void set_parent(RenderTarget* parent) override;
		const RenderTarget* get_parent() const override;
		bool has_parent() const override;

		u32 get_layer_count() const override;
		u32 get_face_count() const override;

		VkImage image() const;
		VkImageView image_view() const;

		u32 width() const override;
		u32 height() const override;

		u32 mip_levels() const override;
		VkSampleCountFlagBits num_samples() const;
		bool transient() const;

		TextureMetaData meta_data() const override;

	private:
		VkImageView generate_view() const;

		VulkanBackend* m_backend;
		RenderTarget* m_parent;

		VkImage m_image;
		VkDeviceMemory m_image_memory;
		VkImageLayout m_image_layout;
		VkImageView m_view;
		u32 m_mipmap_count;
		VkSampleCountFlagBits m_num_samples;
		bool m_transient;

		TextureFormat m_format;
		TextureTiling m_tiling;
		TextureType m_type;

		u32 m_width;
		u32 m_height;
	};
}

#endif // VK_TEXTURE_H_

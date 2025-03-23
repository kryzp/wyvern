#include <backend/graphics/vulkan/vk_texture_mgr.h>
#include <backend/graphics/vulkan/vk_texture.h>
#include <backend/graphics/vulkan/vk_texture_sampler.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <wvn/graphics/image.h>
#include <wvn/graphics/gpu_buffer.h>
#include <wvn/graphics/gpu_buffer_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

VulkanTextureMgr::VulkanTextureMgr(VulkanBackend* backend)
	: m_backend(backend)
{
}

VulkanTextureMgr::~VulkanTextureMgr()
{
}

Texture* VulkanTextureMgr::create(const Image& image)
{
	VulkanTexture* texture = new VulkanTexture(m_backend);

	texture->from_image(image, TEX_TYPE_2D, 4, VK_SAMPLE_COUNT_1_BIT);

	texture->set_mipmapped(true);

	texture->create_internal_resources();

	texture->transition_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	GPUBuffer* stage = GPUBufferMgr::get_singleton()->create_staging_buffer(image.size());
	stage->read_data_from_memory(image.data(), image.size(), 0);
	stage->write_to_tex(texture, image.size());
	delete stage;

	return texture;
}

Texture* VulkanTextureMgr::create(u32 width, u32 height, TextureFormat format, TextureTiling tiling, const byte* data, u64 size)
{
	VulkanTexture* texture = new VulkanTexture(m_backend);

	texture->init_size(width, height);
	texture->init_metadata(format, tiling, TEX_TYPE_2D);
	texture->init_mip_levels(4);

	texture->create_internal_resources();

    texture->transition_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	if (data)
	{
		texture->set_mipmapped(true);

		GPUBuffer* stage = GPUBufferMgr::get_singleton()->create_staging_buffer(size);
		stage->read_data_from_memory(data, size, 0);
		stage->write_to_tex(texture, size);
		delete stage;
	}
	else
	{
		texture->transition_layout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	return texture;
}

Texture* VulkanTextureMgr::create_attachment(u32 width, u32 height, TextureFormat format, TextureTiling tiling)
{
	VulkanTexture* texture = new VulkanTexture(m_backend);

	texture->init_size(width, height);
	texture->init_metadata(format, tiling, TEX_TYPE_2D);

	texture->create_internal_resources();

	texture->transition_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	texture->transition_layout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	return texture;
}

Texture* VulkanTextureMgr::create_cube_map(TextureFormat format, const Image& right, const Image& left, const Image& top, const Image& bottom, const Image& front, const Image& back)
{
	VulkanTexture* texture = new VulkanTexture(m_backend);

	texture->init_size(right.width(), right.height());
	texture->init_metadata(format, TEX_TILE_OPTIMAL, TEX_TYPE_CUBE);
	texture->init_mip_levels(4);

	texture->set_mipmapped(true);

	texture->create_internal_resources();

	texture->transition_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VulkanBuffer* stage = (VulkanBuffer*)GPUBufferMgr::get_singleton()->create_staging_buffer(right.size() * 6);

	const Image* sides[] = { &right, &left, &top, &bottom, &front, &back };

	for (int i = 0; i < 6; i++) {
		stage->read_data_from_memory(sides[i]->data(), sides[i]->size(), sides[i]->size() * i);
		stage->write_to_tex(texture, sides[i]->size(), sides[i]->size() * i, i);
	}

	delete stage;

	return texture;
}

TextureSampler* VulkanTextureMgr::create_sampler(const TextureSampler::Style& style)
{
	return new VulkanTextureSampler(style);
}

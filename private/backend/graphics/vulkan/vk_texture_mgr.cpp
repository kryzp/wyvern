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
	, m_textures()
	, m_samplers()
{
}

VulkanTextureMgr::~VulkanTextureMgr()
{
	for (auto& t : m_textures) {
		delete t;
	}

	for (auto& s : m_samplers) {
		delete s;
	}
}

Texture* VulkanTextureMgr::create(const Image& image)
{
	VulkanTexture* texture = new VulkanTexture(m_backend);
	texture->create(image, TEX_TYPE_2D, 4, VK_SAMPLE_COUNT_1_BIT, false);
	texture->transition_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	GPUBuffer* stage = GPUBufferMgr::get_singleton()->create_staging_buffer(image.size());
	stage->read_data(image.data(), image.size(), 0);
	stage->write_to_tex(texture, image.size());
	delete stage;

	texture->generate_mipmaps();

	m_textures.push_back(texture);
	return texture;
}

Texture* VulkanTextureMgr::create(u32 width, u32 height, TextureFormat format, TextureTiling tiling, const byte* data, u64 size)
{
	VulkanTexture* texture = new VulkanTexture(m_backend);
	texture->create(width, height, format, tiling, TEX_TYPE_2D, 4, VK_SAMPLE_COUNT_1_BIT, false);
	texture->transition_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	GPUBuffer* stage = GPUBufferMgr::get_singleton()->create_staging_buffer(size);
	stage->read_data(data, size, 0);
	stage->write_to_tex(texture, size);
	delete stage;

	texture->generate_mipmaps();

	m_textures.push_back(texture);
	return texture;
}

Texture* VulkanTextureMgr::create_cube_map(TextureFormat format, const Image& right, const Image& left, const Image& top, const Image& bottom, const Image& front, const Image& back)
{
	VulkanTexture* texture = new VulkanTexture(m_backend);
	texture->create(right.width(), right.height(), format, TEX_TILE_OPTIMAL, TEX_TYPE_CUBE, 4, VK_SAMPLE_COUNT_1_BIT, false);
	texture->transition_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VulkanBuffer* stage = (VulkanBuffer*)GPUBufferMgr::get_singleton()->create_staging_buffer(right.size());

	stage->read_data(right.data(), right.size(), 0);
	stage->write_to_tex(texture, right.size(), 0, 0);

	stage->read_data(left.data(), left.size(), 0);
	stage->write_to_tex(texture, left.size(), 0, 1);

	stage->read_data(top.data(), top.size(), 0);
	stage->write_to_tex(texture, top.size(), 0, 2);

	stage->read_data(bottom.data(), bottom.size(), 0);
	stage->write_to_tex(texture, bottom.size(), 0, 3);

	stage->read_data(front.data(), front.size(), 0);
	stage->write_to_tex(texture, front.size(), 0, 4);

	stage->read_data(back.data(), back.size(), 0);
	stage->write_to_tex(texture, back.size(), 0, 5);

	delete stage;

	texture->generate_mipmaps();

	m_textures.push_back(texture);
	return texture;
}

TextureSampler* VulkanTextureMgr::create_sampler(const TextureSampler::Style& style)
{
	VulkanTextureSampler* sampler = new VulkanTextureSampler(style);
	m_samplers.push_back(sampler);
	return sampler;
}

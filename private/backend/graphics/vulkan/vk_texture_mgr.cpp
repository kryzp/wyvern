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
	texture->create(image, 4, VK_SAMPLE_COUNT_1_BIT, false);

	// todo: temp: just to get something working
	GPUBuffer* stage = GPUBufferMgr::get_singleton()->create_staging_buffer(image.size());
	{
		stage->read_data(image.raw_pixel_data(), image.size());
		texture->transition_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		stage->write_to_tex(texture, image.size());
	}
	delete stage;

	texture->generate_mipmaps(); // transition to shader read only happens here!

	m_textures.push_back(texture);

	return texture;
}

Texture* VulkanTextureMgr::create(u32 width, u32 height, TextureFormat format, TextureTiling tiling, const byte* data, u64 size)
{
	VulkanTexture* texture = new VulkanTexture(m_backend);
	texture->create(width, height, format, tiling, 8, VK_SAMPLE_COUNT_1_BIT, false);

	// todo: temp: just to get something working
	GPUBuffer* stage = GPUBufferMgr::get_singleton()->create_staging_buffer(size);
	{
		stage->read_data(data, size);
		texture->transition_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		stage->write_to_tex(texture, size);
	}
	delete stage;

	texture->generate_mipmaps(); // transition to shader read only happens here!

	m_textures.push_back(texture);

	return texture;
}

TextureSampler* VulkanTextureMgr::create_sampler(const TextureSampler::Style& style)
{
	VulkanTextureSampler* sampler = new VulkanTextureSampler(style);
	m_samplers.push_back(sampler);
	return sampler;
}

#include <backend/graphics/vulkan/vk_texture_mgr.h>
#include <backend/graphics/vulkan/vk_texture.h>
#include <backend/graphics/vulkan/vk_backend.h>
#include <wvn/graphics/image.h>
#include <wvn/graphics/gpu_buffer.h>
#include <wvn/graphics/gpu_buffer_mgr.h>

// todo: the actual "management" of the textures here is really primitive. just for prototyping and getting something drawing right now...

using namespace wvn;
using namespace wvn::gfx;

VulkanTextureMgr::VulkanTextureMgr(VulkanBackend* backend)
	: m_backend(backend)
	, m_textures()
{
}

VulkanTextureMgr::~VulkanTextureMgr()
{
	for (auto & m_texture : m_textures) {
		delete m_texture;
	}
}

Texture* VulkanTextureMgr::create(const Image& image)
{
	VulkanTexture* texture = new VulkanTexture();
	texture->init(m_backend);
	texture->create(image);

	// todo: temp: just to get something working
	GPUBuffer* stage = GPUBufferMgr::get_singleton()->create_staging_buffer(image.size());
	{
		stage->write_data(image.raw_pixel_data(), image.size());
		texture->transition_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		stage->write_to_tex(texture, image.size());
		texture->transition_layout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
	delete stage;

	m_textures.push_back(texture);

	return texture;
}

Texture* VulkanTextureMgr::create(u32 width, u32 height, TextureFormat format, TextureTiling tiling, const byte* data, u64 size)
{
	VulkanTexture* texture = new VulkanTexture();
	texture->init(m_backend);
	texture->create(width, height, format, tiling);

	// todo: temp: just to get something working
	GPUBuffer* stage = GPUBufferMgr::get_singleton()->create_staging_buffer(size);
	{
		stage->write_data(data, size);
		texture->transition_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		stage->write_to_tex(texture, size);
		texture->transition_layout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
	delete stage;

	m_textures.push_back(texture);

	return texture;
}

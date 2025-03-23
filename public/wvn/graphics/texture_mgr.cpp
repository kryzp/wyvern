#include <wvn/graphics/texture_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

wvn_IMPL_SINGLETON(TextureMgr);

TextureMgr::TextureMgr()
	: m_texture_cache()
	, m_sampler_cache()
{
}

TextureMgr::~TextureMgr()
{
	for (auto& [id, texture] : m_texture_cache) {
		delete texture;
	}

	m_texture_cache.clear();

	for (auto& [id, sampler] : m_sampler_cache) {
		delete sampler;
	}

	m_sampler_cache.clear();
}

Texture* TextureMgr::get_texture(const String& name)
{
	if (m_texture_cache.contains(name)) {
		return m_texture_cache[name];
	}

	return nullptr;
}

Texture* TextureMgr::register_image_texture(const String& name, const Image& image)
{
	if (m_texture_cache.contains(name)) {
		return m_texture_cache[name];
	}

	Texture* texture = create(image);
	m_texture_cache.insert(Pair(name, texture));

	return texture;
}

Texture* TextureMgr::register_texture(const String& name, u32 width, u32 height, TextureFormat format, TextureTiling tiling, const byte* data, u64 size)
{
	if (m_texture_cache.contains(name)) {
		return m_texture_cache[name];
	}

	Texture* texture = create(width, height, format, tiling, data, size);
	m_texture_cache.insert(Pair(name, texture));

	return texture;
}

Texture* TextureMgr::register_attachment(const String& name, u32 width, u32 height, TextureFormat format, TextureTiling tiling)
{
	if (m_texture_cache.contains(name)) {
		return m_texture_cache[name];
	}

	Texture* texture = create_attachment(width, height, format, tiling);
	m_texture_cache.insert(Pair(name, texture));

	return texture;
}

Texture* TextureMgr::register_cube_map(const String& name, TextureFormat format, const Image& right, const Image& left, const Image& top, const Image& bottom, const Image& front, const Image& back)
{
	if (m_texture_cache.contains(name)) {
		return m_texture_cache[name];
	}

	Texture* texture = create_cube_map(format, right, left, top, bottom, front, back);
	m_texture_cache.insert(Pair(name, texture));

	return texture;
}

TextureSampler* TextureMgr::get_sampler(const String& name)
{
	if (m_sampler_cache.contains(name)) {
		return m_sampler_cache[name];
	}

	return nullptr;
}

TextureSampler* TextureMgr::register_sampler(const String& name, const TextureSampler::Style& style)
{
	if (m_sampler_cache.contains(name)) {
		return m_sampler_cache[name];
	}

	TextureSampler* sampler = create_sampler(style);
	m_sampler_cache.insert(Pair(name, sampler));

	return sampler;
}

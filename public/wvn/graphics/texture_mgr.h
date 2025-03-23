#ifndef TEXTURE_MGR_H_
#define TEXTURE_MGR_H_

#include <wvn/singleton.h>
#include <wvn/common.h>
#include <wvn/container/vector.h>
#include <wvn/container/hash_map.h>
#include <wvn/container/string.h>
#include <wvn/graphics/texture.h>
#include <wvn/graphics/image.h>

namespace wvn::gfx
{
	/**
	 * Responsible for the management of textures.
	 */
	class TextureMgr : public Singleton<TextureMgr>
	{
		wvn_DEF_SINGLETON(TextureMgr);

	public:
		TextureMgr();
		virtual ~TextureMgr();

		Texture* get_texture(const String& name);

		Texture* register_image_texture(const String& name, const Image& image);
		Texture* register_texture(const String& name, u32 width, u32 height, TextureFormat format, TextureTiling tiling, const byte* data, u64 size);
		Texture* register_attachment(const String& name, u32 width, u32 height, TextureFormat format, TextureTiling tiling);
		Texture* register_cube_map(const String& name, TextureFormat format, const Image& right, const Image& left, const Image& top, const Image& bottom, const Image& front, const Image& back);

		virtual Texture* create(const Image& image) = 0;
		virtual Texture* create(u32 width, u32 height, TextureFormat format, TextureTiling tiling, const byte* data, u64 size) = 0;
		virtual Texture* create_attachment(u32 width, u32 height, TextureFormat format, TextureTiling tiling) = 0;
		virtual Texture* create_cube_map(TextureFormat format, const Image& right, const Image& left, const Image& top, const Image& bottom, const Image& front, const Image& back) = 0;

		TextureSampler* get_sampler(const String& name);
		TextureSampler* register_sampler(const String& name, const TextureSampler::Style& style);
		virtual TextureSampler* create_sampler(const TextureSampler::Style& style) = 0;

	private:
		HashMap<String, Texture*> m_texture_cache;
		HashMap<String, TextureSampler*> m_sampler_cache;
	};
}
#endif // TEXTURE_MGR_H_

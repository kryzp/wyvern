#ifndef TEXTURE_MGR_H
#define TEXTURE_MGR_H

#include <wvn/singleton.h>
#include <wvn/common.h>
#include <wvn/graphics/texture.h>
#include <wvn/graphics/image.h>

namespace wvn::gfx
{
	/**
	 * Responsible for the management of textures.
	 */
	class TextureMgr : public Singleton<TextureMgr>
	{
		WVN_DEF_SINGLETON(TextureMgr);

	public:
		TextureMgr();
		virtual ~TextureMgr();

		virtual Texture* create(const Image& image) = 0;
		virtual Texture* create(u32 width, u32 height, TextureFormat format, TextureTiling tiling, const byte* data, u64 size) = 0;
		virtual Texture* create_cube_map(TextureFormat format, const Image& right, const Image& left, const Image& top, const Image& bottom, const Image& front, const Image& back) = 0;

		virtual TextureSampler* create_sampler(const TextureSampler::Style& style) = 0;
	};
}
#endif // TEXTURE_MGR_H

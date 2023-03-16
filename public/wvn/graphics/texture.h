#ifndef TEXTURE_H
#define TEXTURE_H

#include <wvn/util/types.h>
#include <wvn/graphics/texture_sampler.h>

namespace wvn::gfx
{
	class Image;

	enum TextureFormat
	{
		TEX_FMT_NONE = 0,
		TEX_FMT_R8G8B8A8_SRGB,
		TEX_FMT_MAX
	};

	enum TextureTiling
	{
		TEX_TILE_NONE = 0,
		TEX_TILE_OPTIMAL,
		TEX_TILE_LINEAR,
		TEX_TILE_MAX
	};

	/**
	 * Generic texture class for representing, storing and manipulating a texture.
	 */
	class Texture
	{
	public:
		Texture();
		virtual ~Texture();

		virtual void create(const Image& image) = 0;
		virtual void create(u32 width, u32 height, TextureFormat format, TextureTiling tiling) = 0;
	};
}

#endif // TEXTURE_H

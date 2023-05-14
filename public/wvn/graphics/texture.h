#ifndef TEXTURE_H
#define TEXTURE_H

#include <wvn/util/common.h>
#include <wvn/graphics/texture_sampler.h>

namespace wvn::gfx
{
	class Image;

	enum TextureFormat
	{
		TEX_FMT_NONE = 0,
		TEX_FMT_R8G8B8A8_SRGB,
		TEX_FMT_B8G8R8A8_SRGB,
		TEX_FMT_R8_UNORM,
		TEX_FMT_D32_SFLOAT,
		TEX_FMT_D32_SFLOAT_S8_UINT,
		TEX_FMT_D24_UNORM_S8_UINT,
		TEX_FMT_MAX
	};

	enum TextureTiling
	{
		TEX_TILE_NONE = 0,
		TEX_TILE_OPTIMAL,
		TEX_TILE_LINEAR,
		TEX_TILE_MAX
	};

	enum TextureUsage
	{
	};

	struct TextureMetaData
	{
		TextureFormat format;
		TextureTiling tiling;
	};

	/**
	 * Generic texture class for representing, storing and manipulating a texture.
	 */
	class Texture
	{
	public:
		Texture();
		virtual ~Texture();

		virtual u32 width() const = 0;
		virtual u32 height() const = 0;
		virtual TextureMetaData meta_data() const = 0;
	};
}

#endif // TEXTURE_H

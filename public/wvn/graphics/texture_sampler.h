#ifndef TEXTURE_SAMPLER_H
#define TEXTURE_SAMPLER_H

namespace wvn::gfx
{
	enum TextureFilter
	{
		TEX_FILTER_NONE = 0,
		TEX_FILTER_NEAREST,
		TEX_FILTER_LINEAR,
		TEX_FILTER_MAX
	};

	enum TextureWrap
	{
		TEX_WRAP_NONE = 0,
		TEX_WRAP_CLAMP,
		TEX_WRAP_REPEAT,
		TEX_WRAP_MAX
	};

	struct TextureSampler
	{
	public:
		struct Style
		{
			TextureFilter filter;
			TextureWrap wrap_x;
			TextureWrap wrap_y;
			TextureWrap wrap_z;

			Style()
				: filter(TEX_FILTER_NONE)
				, wrap_x(TEX_WRAP_NONE)
				, wrap_y(TEX_WRAP_NONE)
				, wrap_z(TEX_WRAP_NONE)
			{
			}

			Style(TextureFilter filter)
				: filter(filter)
				, wrap_x(TEX_WRAP_CLAMP)
				, wrap_y(TEX_WRAP_CLAMP)
				, wrap_z(TEX_WRAP_CLAMP)
			{
			}

			Style(TextureFilter filter, TextureWrap wrap_x, TextureWrap wrap_y, TextureWrap wrap_z)
				: filter(filter)
				, wrap_x(wrap_x)
				, wrap_y(wrap_y)
				, wrap_z(wrap_z)
			{
			}

			bool operator == (const Style& other) const { return this->filter == other.filter && this->wrap_x == other.wrap_x && this->wrap_y == other.wrap_y && this->wrap_z == other.wrap_z; }
			bool operator != (const Style& other) const { return !(*this == other); }
		};

		bool dirty;
		Style style;

		TextureSampler()
			: dirty(true)
			, style()
		{
		}

		TextureSampler(const Style& style)
			: dirty(true)
			, style(style)
		{
		}

		bool operator == (const TextureSampler& other) const { return this->style == other.style; }
		bool operator != (const TextureSampler& other) const { return this->style != other.style; }
	};
}

#endif // TEXTURE_SAMPLER_H

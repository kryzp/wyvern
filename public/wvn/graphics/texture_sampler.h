#ifndef TEXTURE_SAMPLER_H_
#define TEXTURE_SAMPLER_H_

namespace wvn::gfx
{
	enum TextureFilter
	{
		TEX_FILTER_NONE = 0,
		TEX_FILTER_NEAREST,
		TEX_FILTER_LINEAR,
		TEX_FILTER_MAX_ENUM
	};

	enum TextureWrap
	{
		TEX_WRAP_NONE = 0,
		TEX_WRAP_CLAMP,
		TEX_WRAP_REPEAT,
		TEX_WRAP_MAX_ENUM
	};

	enum TextureBorderColour
	{
		TEX_BORDER_COLOUR_NONE = 0,
		TEX_BORDER_COLOUR_FLOAT_TRANSPARENT_BLACK,
		TEX_BORDER_COLOUR_INT_TRANSPARENT_BLACK,
		TEX_BORDER_COLOUR_FLOAT_OPAQUE_BLACK,
		TEX_BORDER_COLOUR_INT_OPAQUE_BLACK,
		TEX_BORDER_COLOUR_FLOAT_OPAQUE_WHITE,
		TEX_BORDER_COLOUR_INT_OPAQUE_WHITE,
		TEX_BORDER_COLOUR_MAX_ENUM
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
			TextureBorderColour border_colour;

			Style()
				: filter(TEX_FILTER_NONE)
				, wrap_x(TEX_WRAP_NONE)
				, wrap_y(TEX_WRAP_NONE)
				, wrap_z(TEX_WRAP_NONE)
				, border_colour(TEX_BORDER_COLOUR_INT_OPAQUE_BLACK)
			{
			}

			Style(
				TextureFilter filter,
				TextureWrap wrap_x = TEX_WRAP_CLAMP,
				TextureWrap wrap_y = TEX_WRAP_CLAMP,
				TextureWrap wrap_z = TEX_WRAP_CLAMP,
				TextureBorderColour border_colour = TEX_BORDER_COLOUR_INT_OPAQUE_BLACK
			)
				: filter(filter)
				, wrap_x(wrap_x)
				, wrap_y(wrap_y)
				, wrap_z(wrap_z)
				, border_colour(border_colour)
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

		virtual ~TextureSampler()
		{
		}

		virtual void clean_up() = 0;

		bool operator == (const TextureSampler& other) const { return this->style == other.style; }
		bool operator != (const TextureSampler& other) const { return this->style != other.style; }
	};
}

#endif // TEXTURE_SAMPLER_H_

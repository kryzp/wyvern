#ifndef FONT_H
#define FONT_H

#include <wvn/container/string.h>
#include <wvn/maths/rect.h>

namespace wvn::gfx
{
	class Texture;

	enum FontType
	{
		FONT_TYPE_NONE = 0,
		FONT_TYPE_TTF,
		FONT_TYPE_OTF,
		FONT_TYPE_MAX
	};

	/**
	 * Represents a bitmap font for drawing to the screen.
	 */
	class Font
	{
	public:
		struct Info
		{
			float size;
			int ascent;
			int descent;
			int line_gap;
			RectI bbox;
			FontType type;
		};

		struct Kerning
		{
			int advance;
			int glyph0;
			int glyph1;
		};

		struct Character
		{
			int codepoint;
			RectI bbox;
			float advance_x;
			Vec2F draw_offset;
			Vec2F draw_offset2;
		};

		struct Atlas
		{
			Ref<Texture> texture;
		};

		Font();
		Font(float size, const String& path);
		~Font();

		void load(float size, const String& path);
		void free();

		float string_width(const char* str) const;
		float string_height(const char* str) const;

		int kern_advance(int curr, int next) const;

		const Info& info() const;
		const Atlas& atlas() const;
		const Character& character(int idx) const;

	private:
		Info m_info;
		void* m_internal_info;

		Kerning* m_kerning;
		int m_kerning_count;

		Character* m_characters;
		Atlas m_atlas;
	};
}

#endif // FONT_H

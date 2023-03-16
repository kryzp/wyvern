#include <wvn/graphics/font.h>
#include <wvn/maths/calc.h>
#include <wvn/io/file_stream.h>
#include <wvn/graphics/texture.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <third_party/stb_truetype.h>

#define WVN_FONT_ATLAS_W 1024
#define WVN_FONT_ATLAS_H 512
#define WVN_FONT_ATLAS_SIZE (WVN_FONT_ATLAS_W * WVN_FONT_ATLAS_H)
#define WVN_FONT_CHARCOUNT 256
#define M_INTERNAL_INFO ((stbtt_fontinfo*)m_internal_info)

using namespace wvn;
using namespace wvn::gfx;

Font::Font()
	: m_info()
	, m_internal_info(nullptr)
	, m_kerning(nullptr)
	, m_kerning_count(0)
	, m_characters(nullptr)
	, m_atlas()
{
}

Font::Font(float size, const String& path)
	: Font()
{
	load(size, path);
}

Font::~Font()
{
	free();
}

void Font::load(float size, const String& path)
{
	WVN_ASSERT(!path.empty(), "Path must not be empty.");
	WVN_ASSERT(size > 0.0f, "Size must be greater than 0.");

	FontType type = path.ends_with(".ttf") ? FONT_TYPE_TTF : FONT_TYPE_OTF;

	m_info.size = size;
	m_info.type = type;

	if (type == FONT_TYPE_OTF)
	{
		WVN_ERROR("[GFX:FONT|DEBUG] OTF Loading is currently not supported");
	}
	else
	{
		io::FileStream fs(path.c_str(), "rb");
		byte* ttf_buffer = new byte[fs.size()];
		fs.read(ttf_buffer, fs.size());
		fs.close();

		m_internal_info = new stbtt_fontinfo();

		if (!stbtt_InitFont(M_INTERNAL_INFO, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0))) {
			WVN_ERROR("[GFX:FONT|DEBUG] OTF Loading is currently not supported");
		}

		stbtt_GetFontVMetrics(M_INTERNAL_INFO, &m_info.ascent, &m_info.descent, &m_info.line_gap);

		int x0, y0, x1, y1;
		stbtt_GetFontBoundingBox(M_INTERNAL_INFO, &x0, &y0, &x1, &y1);
		{
			m_info.bbox.x = x0;
			m_info.bbox.y = y0;
			m_info.bbox.w = x1 - x0;
			m_info.bbox.h = y1 - y0;
		}

		// store kerning data
		m_kerning_count = stbtt_GetKerningTableLength(M_INTERNAL_INFO);
		m_kerning = new Kerning[m_kerning_count];
		{
			stbtt_kerningentry* kerning_tables = new stbtt_kerningentry[m_kerning_count];
			stbtt_GetKerningTable(M_INTERNAL_INFO, kerning_tables, m_kerning_count);

			for (int i = 0; i < m_kerning_count; i++)
			{
				m_kerning[i] = {
					.advance = kerning_tables[i].advance,
					.glyph0 = kerning_tables[i].glyph1,
					.glyph1 = kerning_tables[i].glyph2
				};
			}

			delete[] kerning_tables;
		}

		// pack data into texture + store character data
		byte* bitmap = new byte[WVN_FONT_ATLAS_SIZE];
		{
			stbtt_packedchar packed_chars[WVN_FONT_CHARCOUNT];
			stbtt_pack_context pack_context = { 0 };

			if (!stbtt_PackBegin(&pack_context, bitmap, WVN_FONT_ATLAS_W, WVN_FONT_ATLAS_H, WVN_FONT_ATLAS_W, 1, NULL)) {
				WVN_ERROR("[GFX:FONT|DEBUG] Failed to initialize font");
			}

			//stbtt_PackSetOversampling(&pack_context, m_info.oversample_x, m_info.oversample_y);

			if (!stbtt_PackFontRange(&pack_context, ttf_buffer, 0, m_info.size, 0, WVN_FONT_CHARCOUNT, packed_chars)) {
				WVN_ERROR("[GFX:FONT|DEBUG] Failed to pack font");
			}

			stbtt_PackEnd(&pack_context);

			// TODO
			//m_atlas.texture = Texture::create(WVN_FONT_ATLAS_W, WVN_FONT_ATLAS_H, TEX_FMT_RED, TEX_I_FMT_R32F, TEX_TYPE_UNSIGNED_BYTE, bitmap);

			m_characters = new Character[WVN_FONT_CHARCOUNT];

			for (int i = 0; i < WVN_FONT_CHARCOUNT; i++)
			{
				const auto& c = packed_chars[i];

				m_characters[i] = {
					.codepoint = i,
					.bbox = RectI(c.x0, c.y0, c.x1 - c.x0, c.y1 - c.y0),
					.advance_x = c.xadvance,
					.draw_offset = Vec2F(c.xoff, c.yoff),
					.draw_offset2 = Vec2F(c.xoff2, c.yoff2)
				};
			}
		}
		delete[] bitmap;
		delete[] ttf_buffer;
	}
}

void Font::free()
{
	delete M_INTERNAL_INFO;
	delete[] m_kerning;
	delete[] m_characters;
}

int Font::kern_advance(int curr, int next) const
{
	for (int i = 0; i < m_kerning_count; i++)
	{
		auto kern = m_kerning[i];

		if (kern.glyph0 == curr && kern.glyph1 == next) {
			return kern.advance;
		}
	}

	return 0;
}

float Font::string_width(const char* str) const
{
	float result = 0.0f;

	for (int i = 0; i < cstr::length(str); i++) {
		result += m_characters[str[i]].advance_x + kern_advance(str[i], str[i + 1]);
	}

	return result;
}

float Font::string_height(const char* str) const
{
	float hhh = 0.0f;
	float max_bboxh = 0.0f;

	for (int i = 0; i < cstr::length(str); i++)
	{
		auto c = m_characters[str[i]];
		max_bboxh = CalcF::max(max_bboxh, c.bbox.h);
		hhh = CalcF::max(hhh, c.bbox.h + c.draw_offset.y);
	}

	return max_bboxh + hhh;
}

const Font::Info& Font::info() const { return m_info; }
const Font::Atlas& Font::atlas() const { return m_atlas; }

const Font::Character& Font::character(int idx) const
{
	WVN_ASSERT(idx >= 0 && idx < WVN_FONT_CHARCOUNT, "Index must be within bounds of the character array [0 -> 255]");
	return m_characters[idx];
}

#ifndef IMAGE_H
#define IMAGE_H

#include <wvn/container/function.h>
#include <wvn/maths/colour.h>
#include <wvn/maths/rect.h>
#include <wvn/io/stream.h>

namespace wvn::gfx
{
	/**
	 * Bitmap image wrapper.
	 */
	class Image
	{
	public:
		using Brush = Function<Colour(u32, u32)>;

		Image();
		Image(const char* path);
		Image(int width, int height);
		~Image();

		void load(const char* path);
		void free();

		void paint(const Brush& brush);
		void paint(const RectI& rect, const Brush& brush);

		void pixels(const Colour* data);
		void pixels(const Colour* data, u64 pixel_count);
		void pixels(const Colour* data, u64 offset, u64 pixel_count);

		bool save_png(const char* file) const;
		bool save_png(io::Stream& stream) const;
		bool save_jpg(const char* file, int quality) const;
		bool save_jpg(io::Stream& stream, int quality) const;

		Colour pixel_at(u32 x, u32 y) const;

		Colour* pixels();
		const Colour* pixels() const;

		byte* raw_pixel_data();
		const byte* raw_pixel_data() const;

		u32 width() const;
		u32 height() const;
		u64 size() const;
		int nr_channels() const;

	private:
		Colour* m_pixels;

		u32 m_width;
		u32 m_height;
		int m_nr_channels;

		bool m_stbi_management;
	};
}

#endif // IMAGE_H

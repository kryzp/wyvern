#ifndef COLOUR_H
#define COLOUR_H

#include <wvn/common.h>

namespace wvn
{
	/**
	 * Generally used just when finally rendering to the screen as often
	 * graphics API's require a 96-bit RGB floating-point colour instead of 32-bit RGBA.
	 *
	 * Not advised to be used for storing data such as images
	 * as its size is 3x that of the other colour type.
	 */
	struct DisplayColour
	{
		union
		{
			struct
			{
				float r;
				float g;
				float b;
			};

			float data[3];
		};

		DisplayColour();
		DisplayColour(float r, float g, float b);
	};

	/**
	 * Used for representing a 32-bit (0-255 per colour) colour.
	 */
	struct Colour
	{
		union
		{
			struct
			{
				u8 r;
				u8 g;
				u8 b;
				u8 a;
			};

			u8 data[4];
		};

		Colour();
		Colour(u8 r, u8 g, u8 b, u8 a = 255);
		Colour(u32 packed);

		static const Colour& empty();
		static const Colour& black();
		static const Colour& white();
		static const Colour& red();
		static const Colour& green();
		static const Colour& blue();
		static const Colour& yellow();
		static const Colour& magenta();
		static const Colour& cyan();

		static Colour from_hsv(float hue, float sat, float val, u8 alpha = 255);
		static Colour lerp(const Colour& from, const Colour& to, float amount);

		u32 packed() const;

		Colour premultiplied() const;

		DisplayColour display_colour() const;

		void export_to_u8(u8* colours) const;
		void export_to_float(float* colours) const;

		bool operator == (const Colour& other) const;
		bool operator != (const Colour& other) const;

		Colour operator - () const;
		Colour operator * (float factor) const;
		Colour operator / (float factor) const;

		Colour& operator *= (float factor);
		Colour& operator /= (float factor);
	};

	// hamburjger spelling
	using Color = Colour;
}

#endif // COLOUR_H

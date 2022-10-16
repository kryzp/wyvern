#pragma once

#include <wvn/util/types.h>

namespace wvn
{
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

		void premultiply();
		Colour premultiplied() const;

		bool operator == (const Colour& other) const;
		bool operator != (const Colour& other) const;

		Colour operator - () const;
		Colour operator * (float factor) const;
		Colour operator / (float factor) const;

		Colour& operator *= (float factor);
	};

	// hamburjger spelling
	using Color = Colour;
}

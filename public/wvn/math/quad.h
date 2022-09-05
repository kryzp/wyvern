#pragma once

#include <wvn/math/vec2.h>
#include <wvn/math/rect.h>

namespace wvn
{
	struct Quad
	{
		Vec2F a;
		Vec2F b;
		Vec2F c;
		Vec2F d;

		Quad();
		Quad(const Vec2F& a, const Vec2F& b, const Vec2F& c, const Vec2F& d);
		Quad(const RectF& rect);

		Quad operator / (const Vec2F& factor) const;
	};
}

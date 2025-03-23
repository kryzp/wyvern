#ifndef QUAD_H_
#define QUAD_H_

#include <wvn/maths/vec2.h>
#include <wvn/maths/rect.h>

namespace wvn
{
	/**
	 * Representation of a quad, a shape between 4 points.
	 */
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

#endif // QUAD_H_

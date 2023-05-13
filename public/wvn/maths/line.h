#ifndef LINE_H
#define LINE_H

#include <wvn/maths/vec2.h>
#include <wvn/maths/vec3.h>

namespace wvn
{
	/**
	 * Representation of a line between two 2D points.
	 */
	struct Line2D
	{
		Vec2F a;
		Vec2F b;

		Line2D();
		Line2D(const Vec2F& a, const Vec2F& b);
		Line2D(const Vec2F& pos, float angle, float len);

		Vec2F delta() const;
		Vec2F direction() const;
		float length() const;
		float length_squared() const;
		float angle() const;
	};

	/**
	 * Representation of a line between two 3D points.
	 */
	struct Line3D
	{
		Vec3F a;
		Vec3F b;

		Line3D();
		Line3D(const Vec3F& a, const Vec3F& b);

		Vec3F delta() const;
		Vec3F direction() const;
		float length() const;
		float length_squared() const;
	};
}

#endif // LINE_H

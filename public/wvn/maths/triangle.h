#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <wvn/maths/vec3.h>

namespace wvn
{
	struct Triangle
	{
		Vec3F a;
		Vec3F b;
		Vec3F c;

		Triangle();
		Triangle(const Vec3F& a, const Vec3F& b, const Vec3F& c);

		Vec3F centre() const;
	};
}

#endif // TRIANGLE_H_

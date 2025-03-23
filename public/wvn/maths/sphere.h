#ifndef CIRCLE_H_
#define CIRCLE_H_

#include <wvn/maths/vec3.h>

namespace wvn
{
	/**
	 * Representation of a sphere.
	 */
	struct Sphere
	{
		union
		{
			struct
			{
				Vec3F position;
				float radius;
			};

			float data[4];
		};

		Sphere();
		Sphere(float rad);
		Sphere(const Vec3F& pos, float rad);

		static const Sphere& unit();

		float diameter() const;
	};
}

#endif // CIRCLE_H_

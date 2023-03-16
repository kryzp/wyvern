#ifndef CIRCLE_H
#define CIRCLE_H

#include <wvn/maths/vec2.h>

namespace wvn
{
	/**
	 * Representation of a circle.
	 */
	struct Circle
	{
		union
		{
			struct
			{
				Vec2F position;
				float radius;
			};

			float data[3];
		};

		Circle();
		Circle(float rad);
		Circle(const Vec2F& pos, float rad);

		static const Circle& zero();
		static const Circle& one();

		float diameter() const;
	};
}

#endif // CIRCLE_H

#pragma once

#include <wvn/math/vec2.h>

namespace wvn
{
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

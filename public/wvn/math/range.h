#pragma once

#include <wvn/math/random.h>

namespace wvn
{
	struct Range
	{
		float min;
		float max;

		Range()
		{
		}

		Range(float min, float max)
			: min(min)
			, max(max)
		{
		}

		float delta() const
		{
			return max - min;
		}

		float random() const
		{
			return Rand<std::mt19937>::get_singleton()->real(min, max);
		}
	};
}

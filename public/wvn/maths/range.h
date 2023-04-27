#ifndef RANGE_H
#define RANGE_H

#include <wvn/root.h>

namespace wvn
{
	struct Range
	{
		float min;
		float max;

		Range()
			: min(0.0f)
			, max(0.0f)
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
			return Root::get_singleton()->random.real32(min, max);
		}
	};
}

#endif // RANGE_H

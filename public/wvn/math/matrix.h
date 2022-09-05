#pragma once

namespace wvn
{
	template <int TRows, int TColumns>
	struct Matrix
	{
		float elements[TColumns][TRows];
	};
}

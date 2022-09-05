#pragma once

#include <wvn/util/types.h>
#include <wvn/util/singleton.h>

#include <random>
#include <ctime>

namespace wvn
{
	class Random : public Singleton<Random>
	{
		WVN_DEF_SINGLETON(Random);

	public:
		Random();

		int integer(int min, int max);
		float real(float min, float max);

		template <typename TDist, typename T>
		T generic_range(T min, T max);

	private:
		std::mt19937 m_rng;
	};
}

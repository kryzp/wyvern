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
		Random(u64 seed);

		~Random();

		int integer(int min, int max);
		float real32(float min, float max);
		double real64(double min, double max);

		template <typename TDist, typename T>
		T generic_range(T min, T max);

	private:
		std::mt19937 m_rng;
	};

	template <typename TDist, typename T>
	T Random::generic_range(T min, T max)
	{
		TDist dist = TDist(min, max);
		return dist(m_rng);
	}
}

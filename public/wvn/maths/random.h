#ifndef RANDOM_H
#define RANDOM_H

#include <wvn/common.h>

#include <random>
#include <ctime>

namespace wvn
{
	/**
	 * Used for generating random numbers given a seed.
	 */
	template <class Engine = std::mt19937>
	class Random
	{
	public:
		Random();
		Random(u64 seed);

		~Random();

		void regen_seed(u64 seed);

		int integer(int min, int max);
		float real32(float min, float max);
		double real64(double min, double max);

		template <typename TDist, typename T>
		T generic_range(T min, T max);

	private:
		Engine m_rng;
	};

	template <class Engine>
	Random<Engine>::Random()
		: m_rng(std::time(nullptr))
	{
	}

	template <class Engine>
	Random<Engine>::Random(u64 seed)
		: m_rng(seed)
	{
	}

	template <class Engine>
	Random<Engine>::~Random()
	{
	}

	template <class Engine>
	void Random<Engine>::regen_seed(u64 seed)
	{
		m_rng.seed(seed);
	}

	template <class Engine>
	int Random<Engine>::integer(int min, int max)
	{
		return generic_range<std::uniform_int_distribution<int>>(min, max);
	}

	template <class Engine>
	float Random<Engine>::real32(float min, float max)
	{
		return generic_range<std::uniform_real_distribution<float>>(min, max);
	}

	template <class Engine>
	double Random<Engine>::real64(double min, double max)
	{
		return generic_range<std::uniform_real_distribution<double>>(min, max);
	}

	template <class Engine>
	template <typename TDist, typename T>
	T Random<Engine>::generic_range(T min, T max)
	{
		TDist dist = TDist(min, max);
		return dist(m_rng);
	}
}

#endif // RANDOM_H

#include <wvn/math/random.h>

using namespace wvn;

WVN_IMPL_SINGLETON(Random);

Random::Random()
	: m_rng(std::time(nullptr))
{
}

int Random::integer(int min, int max)
{
	return generic_range<std::uniform_int_distribution<int>>(min, max);
}

float Random::real(float min, float max)
{
	return generic_range<std::uniform_real_distribution<float>>(min, max);
}

template <typename TDist, typename T>
T Random::generic_range(T min, T max)
{
	TDist dist = TDist(min, max);
	return dist(m_rng);
}

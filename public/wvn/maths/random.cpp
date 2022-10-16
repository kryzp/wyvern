#include <wvn/maths/random.h>

using namespace wvn;

WVN_IMPL_SINGLETON(Random);

Random::Random()
	: m_rng(std::time(nullptr))
{
}

Random::Random(u64 seed)
	: m_rng(seed)
{
}

Random::~Random()
{
}

int Random::integer(int min, int max)
{
	return generic_range<std::uniform_int_distribution<int>>(min, max);
}

float Random::real32(float min, float max)
{
	return generic_range<std::uniform_real_distribution<float>>(min, max);
}

double Random::real64(double min, double max)
{
	return generic_range<std::uniform_real_distribution<double>>(min, max);
}

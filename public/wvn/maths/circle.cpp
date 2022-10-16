#include <wvn/maths/circle.h>

using namespace wvn;

Circle::Circle()
	: position(Vec2F::zero())
	, radius(0.0f)
{
}

Circle::Circle(float rad)
	: position(Vec2F::zero())
	, radius(rad)
{
}

Circle::Circle(const Vec2F& pos, float rad)
	: position(pos)
	, radius(rad)
{
}

float Circle::diameter() const
{
	return radius * 2.0f;
}

const Circle& Circle::zero() { static const Circle ZERO = Circle(Vec2F::zero(), 0.0f); return ZERO; }
const Circle& Circle::one()  { static const Circle ONE  = Circle(Vec2F::zero(), 1.0f); return ONE;  }

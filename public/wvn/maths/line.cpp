#include <wvn/maths/line.h>
#include <wvn/maths/calc.h>

using namespace wvn;

Line::Line()
	: a(Vec2F::zero())
	, b(Vec2F::zero())
{
}

Line::Line(const Vec2F& a, const Vec2F& b)
	: a(a)
	, b(b)
{
}

Line::Line(const Vec2F& pos, float angle, float len)
	: a(pos)
	, b(pos + Vec2F(CalcF::cos(angle) * len, CalcF::sin(angle) * len))
{
}

Vec2F Line::delta() const
{
	return (b - a);
}

Vec2F Line::direction() const
{
	return (b - a).normalized();
}

float Line::length() const
{
	return (b - a).length();
}

float Line::length_squared() const
{
	return (b - a).length_squared();
}

float Line::angle() const
{
	return (b - a).angle();
}

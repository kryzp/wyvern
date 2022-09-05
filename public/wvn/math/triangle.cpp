#include <wvn/math/triangle.h>

using namespace wvn;

Triangle::Triangle()
	: a(Vec2F::zero())
	, b(Vec2F::zero())
	, c(Vec2F::zero())
{
}

Triangle::Triangle(const Vec2F& a, const Vec2F& b, const Vec2F& c)
	: a(a)
	, b(b)
	, c(c)
{
}

Vec2F Triangle::centre() const
{
	return (a + b + c) / 3.0f;
}

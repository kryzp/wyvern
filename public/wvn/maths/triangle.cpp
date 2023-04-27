#include <wvn/maths/triangle.h>

using namespace wvn;

Triangle::Triangle()
	: a(Vec3F::zero())
	, b(Vec3F::zero())
	, c(Vec3F::zero())
{
}

Triangle::Triangle(const Vec3F& a, const Vec3F& b, const Vec3F& c)
	: a(a) , b(b) , c(c)
{
}

Vec3F Triangle::centre() const
{
	return (a + b + c) / 3.0f;
}

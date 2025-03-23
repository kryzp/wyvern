#include <wvn/maths/line.h>
#include <wvn/maths/calc.h>

using namespace wvn;

Line2D::Line2D()
	: a(Vec2F::zero())
	, b(Vec2F::zero())
{
}

Line2D::Line2D(const Vec2F& a, const Vec2F& b)
	: a(a)
	, b(b)
{
}

Line2D::Line2D(const Vec2F& pos, float angle, float len)
	: a(pos)
	, b(pos + Vec2F(CalcF::cos(angle) * len, CalcF::sin(angle) * len))
{
}

Vec2F Line2D::delta() const
{
	return (b - a);
}

Vec2F Line2D::direction() const
{
	return (b - a).normalized();
}

float Line2D::length() const
{
	return (b - a).length();
}

float Line2D::length_squared() const
{
	return (b - a).length_squared();
}

float Line2D::angle() const
{
	return (b - a).angle();
}

/***************************/

Line3D::Line3D()
	: a(Vec3F::zero())
	, b(Vec3F::zero())
{
}

Line3D::Line3D(const Vec3F& a, const Vec3F& b)
	: a(a)
	, b(b)
{
}

Vec3F Line3D::delta() const
{
	return (b - a);
}

Vec3F Line3D::direction() const
{
	return (b - a).normalized();
}

float Line3D::length() const
{
	return (b - a).length();
}

float Line3D::length_squared() const
{
	return (b - a).length_squared();
}

float Line3D::angle(const Vec3F& up) // assuming |up|=1
{
	return CalcF::asin(Vec3F::dot(direction(), up));
}

float Line3D::angle_xy() const
{
	return CalcF::asin(direction().z);
}

float Line3D::angle_yz() const
{
	return CalcF::asin(direction().x);
}

float Line3D::angle_xz() const
{
	return CalcF::asin(direction().y);
}

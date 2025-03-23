#include <wvn/maths/sphere.h>

using namespace wvn;

Sphere::Sphere()
	: position(0.0f, 0.0f, 0.0f)
	, radius(0.0f)
{
}

Sphere::Sphere(float rad)
	: position(0.0f, 0.0f, 0.0f)
	, radius(rad)
{
}

Sphere::Sphere(const wvn::Vec3F& pos, float rad)
	: position(pos)
	, radius(rad)
{
}

float Sphere::diameter() const
{
	return 2.0f * radius;
}

const Sphere& Sphere::unit() { static const Sphere UNIT = Sphere(1.0f); return UNIT; }

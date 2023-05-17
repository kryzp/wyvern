#include <wvn/maths/quaternion.h>
#include <wvn/maths/calc.h>
#include <wvn/maths/vec3.h>

using namespace wvn;

Quaternion::Quaternion()
	: w(0.0f), x(0.0f), y(0.0f), z(0.0f)
{
}

Quaternion::Quaternion(float x, float y, float z)
	: w(0.0f), x(x), y(y), z(z)
{
}

Quaternion::Quaternion(float w, float x, float y, float z)
	: w(w), x(x), y(y), z(z)
{
}

Quaternion::Quaternion(const Quaternion& other)
		: w(other.w), x(other.x), y(other.y), z(other.z)
{
}

Quaternion Quaternion::from_axis_angle(const Vec3F& axis, float angle)
{
	Quaternion q = Quaternion::zero();
	q.w = CalcF::cos(angle / 2.0f);
	q.x = CalcF::sin(angle / 2.0f) * axis.x;
	q.y = CalcF::sin(angle / 2.0f) * axis.y;
	q.z = CalcF::sin(angle / 2.0f) * axis.z;

	return q;
}

// i think this might be using the wrong coordinate axis...
Quaternion Quaternion::from_euler(float pitch, float yaw, float roll)
{
	float ps = CalcF::sin(pitch / 2.0f);
	float pc = CalcF::cos(pitch / 2.0f);
	float ys = CalcF::sin(yaw / 2.0f);
	float yc = CalcF::cos(yaw / 2.0f);
	float rs = CalcF::sin(roll / 2.0f);
	float rc = CalcF::cos(roll / 2.0f);

	float s = (rs * pc * yc) - (rc * ps * ys);
	float i = (rc * ps * yc) + (rs * pc * ys);
	float j = (rc * pc * ys) - (rs * ps * yc);
	float k = (rc * pc * yc) + (ys * ps * ys);

	return Quaternion(s, i, j, k);
}

Vec3F Quaternion::to_euler(const Quaternion& quat)
{
	float t0 =             (2.0f + ((quat.w * quat.x) + (quat.y * quat.z)));
	float t1 = 1.0f -      (2.0f * ((quat.x * quat.x) + (quat.y * quat.y)));
	float t2 = CalcF::clamp(2.0f * ((quat.w * quat.y) - (quat.z * quat.x)), -1.0f, 1.0f);
	float t3 =             (2.0f * ((quat.w * quat.z) + (quat.x * quat.y)));
	float t4 = 1.0f -      (2.0f * ((quat.y * quat.y) + (quat.z * quat.z)));

	float p = CalcF::asin(t2);
	float y = CalcF::atan2(t3, t4);
	float r = CalcF::atan2(t0, t1);

	return Vec3F(p, y, r);
}

float Quaternion::dot(const Quaternion& a, const Quaternion& b)
{
	return (a.w * b.w) + (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

Vec3F Quaternion::vector() const
{
	return Vec3F(x, y, z);
}

Quaternion Quaternion::normalized() const
{
	float l = length();
	return Quaternion(w / l, x / l, y / l, z / l);
}

Quaternion Quaternion::inverse() const
{
	return Quaternion(w, -x, -y, -z);
}

float Quaternion::length_squared() const
{
	return Quaternion::dot(*this, *this);
}

float Quaternion::length() const
{
	return CalcF::sqrt(length_squared());
}

Quaternion Quaternion::rotate_on_axis(const Vec3F& axis, float angle) const
{
	return (*this) * Quaternion::from_axis_angle(axis, angle);
}

bool Quaternion::operator == (const Quaternion& other) const
{
	return (
		this->w == other.w &&
		this->x == other.x &&
		this->y == other.y &&
		this->z == other.z
	);
}

bool Quaternion::operator != (const Quaternion& other) const
{
	return !(*this == other);
}

Quaternion Quaternion::operator + (const Quaternion& other) const
{
	return Quaternion(
		this->w + other.w,
		this->x + other.x,
		this->y + other.y,
		this->z + other.z
	);
}

Quaternion Quaternion::operator - (const Quaternion& other) const
{
	return Quaternion(
		this->w - other.w,
		this->x - other.x,
		this->y - other.y,
		this->z - other.z
	);
}

Quaternion Quaternion::operator * (const Quaternion& other) const
{
	return Quaternion(
		(this->w * other.w) - (this->x * other.x) - (this->y * other.y) - (this->z * other.z),
		(this->w * other.x) + (this->x * other.w) + (this->y * other.z) - (this->z * other.y),
		(this->w * other.y) - (this->x * other.z) + (this->y * other.w) + (this->z * other.x),
		(this->w * other.z) + (this->x * other.y) - (this->y * other.x) + (this->z * other.w)
	);
}

Quaternion Quaternion::operator * (float scalar) const
{
	return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
}

Quaternion Quaternion::operator / (float scalar) const
{
	return Quaternion(w / scalar, x / scalar, y / scalar, z / scalar);
}

Quaternion Quaternion::operator - () const
{
	return Quaternion(-w, -x, -y, -z);
}

Quaternion& Quaternion::operator += (const Quaternion& other)
{
	(*this) = (*this) + other;
	return *this;
}

Quaternion& Quaternion::operator -= (const Quaternion& other)
{
	(*this) = (*this) - other;
	return *this;
}

Quaternion& Quaternion::operator *= (const Quaternion& other)
{
	(*this) = (*this) * other;
	return *this;
}

const Quaternion& Quaternion::zero()	 { static const Quaternion ZERO      = Quaternion(0.0f, 0.0f, 0.0f, 0.0f); return ZERO;	    }
const Quaternion& Quaternion::one()      { static const Quaternion ONE       = Quaternion(0.0f, 1.0f, 1.0f, 1.0f); return ONE;      }
const Quaternion& Quaternion::identity() { static const Quaternion IDENTITY  = Quaternion(1.0f, 0.0f, 0.0f, 0.0f); return IDENTITY; }

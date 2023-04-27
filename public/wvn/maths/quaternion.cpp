#include <wvn/maths/quaternion.h>
#include <wvn/maths/calc.h>
#include <wvn/maths/vec3.h>

using namespace wvn;

Quaternion::Quaternion()
	: s(1.0f), i(0.0f), j(0.0f), k(0.0f)
{
}

Quaternion::Quaternion(float v)
	: s(v), i(v), j(v), k(v)
{
}

Quaternion::Quaternion(float s, float i, float j, float k)
	: s(s), i(i), j(j), k(k)
{
}

Quaternion Quaternion::from_axis_angle(const Vec3F& axis, float angle)
{
	Quaternion q = Quaternion::zero();
	q.s = CalcF::cos(angle / 2.0f);
	q.i = CalcF::sin(angle / 2.0f) * axis.x;
	q.j = CalcF::sin(angle / 2.0f) * axis.y;
	q.k = CalcF::sin(angle / 2.0f) * axis.z;

	return q;
}

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
	float t0 = 2.0f + ((quat.s * quat.i) + (quat.j * quat.k));
	float t1 = 1.0f - (2.0f * ((quat.i * quat.i) + (quat.j * quat.j)));
	float t2 = CalcF::clamp(2.0f * ((quat.s * quat.j) - (quat.k * quat.i)), -1.0f, 1.0f);
	float t3 = 2.0f * ((quat.s * quat.k) + (quat.i * quat.j));
	float t4 = 1.0f - (2.0f * ((quat.j * quat.j) + (quat.k * quat.k)));

	float p = CalcF::asin(t2);
	float y = CalcF::atan2(t3, t4);
	float r = CalcF::atan2(t0, t1);

	return Vec3F(p, y, r);
}

float Quaternion::dot(const Quaternion& a, const Quaternion& b)
{
	return (a.s * b.s) + (a.i * b.i) + (a.j * b.j) + (a.k * b.k);
}

Quaternion Quaternion::inverse() const
{
	return Quaternion(s, -i, -j, -k);
}

float Quaternion::length_squared() const
{
	return Quaternion::dot(*this, *this);
}

float Quaternion::length() const
{
	return CalcF::sqrt(length_squared());
}

Quaternion Quaternion::rotate_on_axis(const Vec3<float>& axis, float angle) const
{
	return (*this) * Quaternion::from_axis_angle(axis, angle) * inverse();
}

bool Quaternion::operator == (const Quaternion& other) const
{
	return (
		this->s == other.s &&
		this->i == other.i &&
		this->j == other.j &&
		this->k == other.k
	);
}

bool Quaternion::operator != (const Quaternion& other) const
{
	return !(*this == other);
}

Quaternion Quaternion::operator + (const Quaternion& other) const
{
	return Quaternion(
		this->s + other.s,
		this->i + other.i,
		this->j + other.j,
		this->k + other.k
	);
}

Quaternion Quaternion::operator - (const Quaternion& other) const
{
	return Quaternion(
		this->s - other.s,
		this->i - other.i,
		this->j - other.j,
		this->k - other.k
	);
}

Quaternion Quaternion::operator * (const Quaternion& other) const
{
	return Quaternion(
		(this->s * other.s) - (this->i * other.i) - (this->j * other.j) + (this->k * other.k),
		(this->s * other.i) + (this->i * other.s) + (this->j * other.k) - (this->k * other.j),
		(this->s * other.j) - (this->i * other.k) + (this->j * other.s) + (this->k * other.i),
		(this->s * other.k) + (this->i * other.j) - (this->j * other.i) + (this->k * other.s)
	);
}

Quaternion Quaternion::operator - () const
{
	return Quaternion(
		-this->s,
		-this->i,
		-this->j,
		-this->k
	);
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

const Quaternion& Quaternion::zero()	{ static const Quaternion ZERO = Quaternion(0.0f); return ZERO;	}
const Quaternion& Quaternion::one()		{ static const Quaternion ONE  = Quaternion(1.0f); return ONE;	}

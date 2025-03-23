#include <wvn/maths/quat.h>
#include <wvn/maths/calc.h>
#include <wvn/maths/vec3.h>

using namespace wvn;

Quat::Quat()
	: w(0.0f), x(0.0f), y(0.0f), z(0.0f)
{
}

Quat::Quat(const Vec3F& xyz)
	: w(0.0f), x(xyz.x), y(xyz.y), z(xyz.z)
{
}

Quat::Quat(float x, float y, float z)
	: w(0.0f), x(x), y(y), z(z)
{
}

Quat::Quat(float w, float x, float y, float z)
	: w(w), x(x), y(y), z(z)
{
}

Quat::Quat(const Quat& other)
		: w(other.w), x(other.x), y(other.y), z(other.z)
{
}

Quat Quat::from_axis_angle(const Vec3F& axis, float angle)
{
	Quat q = Quat::zero();
	q.w = CalcF::cos(angle / 2.0f);
	q.x = CalcF::sin(angle / 2.0f) * axis.x;
	q.y = CalcF::sin(angle / 2.0f) * axis.y;
	q.z = CalcF::sin(angle / 2.0f) * axis.z;

	return q;
}

Quat Quat::from_euler(float pitch, float yaw, float roll)
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

	return Quat(s, i, j, k);
}

Vec3F Quat::to_euler(const Quat& quat)
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

float Quat::dot(const Quat& a, const Quat& b)
{
	return (a.w * b.w) + (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

Vec3F Quat::vector() const
{
	return Vec3F(x, y, z);
}

Quat Quat::normalized() const
{
	return (*this) / length();
}

Quat Quat::inverse() const
{
	Quat ret = Quat::identity();
	float prod = length_squared();

	if (prod > CalcF::epsilon())
	{
		ret.w =  this->w / CalcF::sqrt(prod);
		ret.x = -this->x / CalcF::sqrt(prod);
		ret.y = -this->y / CalcF::sqrt(prod);
		ret.z = -this->z / CalcF::sqrt(prod);
	}
	else
	{
		ret.w =  this->w;
		ret.x = -this->x;
		ret.y = -this->y;
		ret.z = -this->z;
	}

	return ret;
}

float Quat::length_squared() const
{
	return Quat::dot(*this, *this);
}

float Quat::length() const
{
	return CalcF::sqrt(length_squared());
}

Quat Quat::rotate_on_axis(const Vec3F& axis, float angle) const
{
	return (*this) * Quat::from_axis_angle(axis, angle);
}

bool Quat::operator == (const Quat& other) const
{
	return (
		this->w == other.w &&
		this->x == other.x &&
		this->y == other.y &&
		this->z == other.z
	);
}

bool Quat::operator != (const Quat& other) const
{
	return !(*this == other);
}

Quat Quat::operator + (const Quat& other) const
{
	return Quat(
		this->w + other.w,
		this->x + other.x,
		this->y + other.y,
		this->z + other.z
	);
}

Quat Quat::operator - (const Quat& other) const
{
	return Quat(
		this->w - other.w,
		this->x - other.x,
		this->y - other.y,
		this->z - other.z
	);
}

Quat Quat::operator * (const Quat& other) const
{
	return Quat(
		(this->w * other.w) - (this->x * other.x) - (this->y * other.y) - (this->z * other.z),
		(this->w * other.x) + (this->x * other.w) + (this->y * other.z) - (this->z * other.y),
		(this->w * other.y) - (this->x * other.z) + (this->y * other.w) + (this->z * other.x),
		(this->w * other.z) + (this->x * other.y) - (this->y * other.x) + (this->z * other.w)
	);
}

Quat Quat::operator / (float scalar) const
{
	return Quat(w / scalar, x / scalar, y / scalar, z / scalar);
}

Quat Quat::operator - () const
{
	return Quat(-w, -x, -y, -z);
}

Quat& Quat::operator += (const Quat& other)
{
	(*this) = (*this) + other;
	return *this;
}

Quat& Quat::operator -= (const Quat& other)
{
	(*this) = (*this) - other;
	return *this;
}

Quat& Quat::operator *= (const Quat& other)
{
	(*this) = (*this) * other;
	return *this;
}

const Quat& Quat::zero()	 { static const Quat ZERO      = Quat(0.0f, 0.0f, 0.0f, 0.0f); return ZERO;	    }
const Quat& Quat::one()      { static const Quat ONE       = Quat(0.0f, 1.0f, 1.0f, 1.0f); return ONE;      }
const Quat& Quat::identity() { static const Quat IDENTITY  = Quat(1.0f, 0.0f, 0.0f, 0.0f); return IDENTITY; }

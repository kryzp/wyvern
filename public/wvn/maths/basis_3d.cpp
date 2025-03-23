#include <wvn/maths/basis_3d.h>
#include <wvn/maths/mat4x4.h>
#include <wvn/maths/vec3.h>
#include <wvn/maths/quat.h>
#include <wvn/maths/calc.h>

using namespace wvn;

Basis3D::Basis3D()
	: m11(0), m12(0), m13(0)
	, m21(0), m22(0), m23(0)
	, m31(0), m32(0), m33(0)
{
}

Basis3D::Basis3D(const Basis3D& other)
	: m11(other.m11), m12(other.m12), m13(other.m13)
	, m21(other.m21), m22(other.m22), m23(other.m23)
	, m31(other.m31), m32(other.m32), m33(other.m33)
{
}

Basis3D::Basis3D(float diag)
	: m11(diag), m12(0), m13(0)
	, m21(0), m22(diag), m23(0)
	, m31(0), m32(0), m33(diag)
{
}

Basis3D::Basis3D(
	float m11, float m12, float m13,
	float m21, float m22, float m23,
	float m31, float m32, float m33
)
	: m11(m11), m12(m12), m13(m13)
	, m21(m21), m22(m22), m23(m23)
	, m31(m31), m32(m32), m33(m33)
{
}

float Basis3D::determinant() const
{
	float d1 = (m33 * m22) - (m32 * m23);
	float d2 = (m33 * m12) - (m32 * m13);
	float d3 = (m23 * m12) - (m22 * m13);

	return (m11 * d1) - (m21 * d2) + (m31 * d3);
}

Basis3D Basis3D::inverse() const
{
	float det = determinant();

	return Basis3D(
		((m22 * m33) - (m32 * m23)) / det,
		((m13 * m32) - (m12 * m33)) / det,
		((m12 * m23) - (m13 * m22)) / det,

		((m23 * m31) - (m21 * m33)) / det,
		((m11 * m33) - (m13 * m31)) / det,
		((m21 * m13) - (m11 * m23)) / det,

		((m21 * m32) - (m31 * m22)) / det,
		((m31 * m12) - (m11 * m32)) / det,
		((m11 * m22) - (m21 * m12)) / det
	);
}

Basis3D Basis3D::transpose() const
{
	return Basis3D(
		m11, m21, m31,
		m12, m22, m32,
		m13, m23, m33
	);
}

Mat4x4 Basis3D::as4x4() const
{
	return Mat4x4(
		m11, m12, m13, 0.0f,
		m21, m22, m23, 0.0f,
		m31, m32, m33, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

Vec3F Basis3D::transform(const Vec3F& vec, const Basis3D& mat)
{
	return Vec3(
		(vec.x * mat.m11) + (vec.y * mat.m21) + (vec.z * mat.m31),
		(vec.x * mat.m12) + (vec.y * mat.m22) + (vec.z * mat.m32),
		(vec.x * mat.m13) + (vec.y * mat.m23) + (vec.z * mat.m33)
	);
}

Basis3D Basis3D::create_skew(const Vec3F& skew)
{
	return Basis3D(
		0.0f, 0.0f, skew.x,
		0.0f, skew.y, 0.0f,
		skew.z, 0.0f, 0.0f
	);
}

Basis3D Basis3D::create_scale(const Vec3F& scale)
{
	return Basis3D(
		scale.x, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f,
		0.0f, 0.0f, scale.z
	);
}

Basis3D Basis3D::create_scale(float x, float y, float z)
{
	return Basis3D(
		x, 0.0f, 0.0f,
		0.0f, y, 0.0f,
		0.0f, 0.0f, z
	);
}

Basis3D Basis3D::create_scale(float scale)
{
	return Basis3D(
		scale, 0.0f, 0.0f,
		0.0f, scale, 0.0f,
		0.0f, 0.0f, scale
	);
}

Basis3D Basis3D::create_rotation(const wvn::Quat& quat)
{
	return Basis3D(
		1.0f - 2.0f * (quat.y * quat.y + quat.z * quat.z),
			   2.0f * (quat.x * quat.y - quat.z * quat.w),
			   2.0f * (quat.x * quat.z + quat.y * quat.w),

			   2.0f * (quat.x * quat.y + quat.z * quat.w),
		1.0f - 2.0f * (quat.x * quat.x + quat.z * quat.z),
			   2.0f * (quat.y * quat.z - quat.x * quat.w),

			   2.0f * (quat.x * quat.z - quat.y * quat.w),
			   2.0f * (quat.y * quat.z + quat.x * quat.w),
		1.0f - 2.0f * (quat.x * quat.x + quat.y * quat.y)
	);
}

Basis3D Basis3D::create_rotation(const Vec3F& axis, float angle)
{
	float sin_theta = CalcF::sin(angle);
	float cos_theta = CalcF::cos(angle);
	float cos_inv   = 1.0f - cos_theta;

	return Basis3D(
		 (axis.x * axis.x * cos_inv) +           cos_theta,
		-(axis.x * axis.y * cos_inv) + (axis.z * sin_theta),
		 (axis.x * axis.z * cos_inv) + (axis.y * sin_theta),

		-(axis.y * axis.x * cos_inv) - (axis.z * sin_theta),
		 (axis.y * axis.y * cos_inv) +           cos_theta,
		-(axis.y * axis.z * cos_inv) + (axis.x * sin_theta),

		 (axis.z * axis.x * cos_inv) - (axis.y * sin_theta),
		-(axis.z * axis.y * cos_inv) - (axis.x * sin_theta),
		 (axis.z * axis.z * cos_inv) +           cos_theta
	);
}

Basis3D Basis3D::operator - (const Basis3D& other) const
{
	return Basis3D(
		this->m11 - other.m11,
		this->m12 - other.m12,
		this->m13 - other.m13,

		this->m21 - other.m21,
		this->m22 - other.m22,
		this->m23 - other.m23,

		this->m31 - other.m31,
		this->m32 - other.m32,
		this->m33 - other.m33
	);
}

Basis3D Basis3D::operator + (const Basis3D& other) const
{
	return Basis3D(
		this->m11 + other.m11,
		this->m12 + other.m12,
		this->m13 + other.m13,

		this->m21 + other.m21,
		this->m22 + other.m22,
		this->m23 + other.m23,

		this->m31 + other.m31,
		this->m32 + other.m32,
		this->m33 + other.m33
	);
}

Basis3D Basis3D::operator / (float scalar) const
{
	return Basis3D(
		this->m11 / scalar,
		this->m12 / scalar,
		this->m13 / scalar,

		this->m21 / scalar,
		this->m22 / scalar,
		this->m23 / scalar,

		this->m31 / scalar,
		this->m32 / scalar,
		this->m33 / scalar
	);
}

Basis3D Basis3D::operator * (float scalar) const
{
	return Basis3D(
		this->m11 * scalar,
		this->m12 * scalar,
		this->m13 * scalar,

		this->m21 * scalar,
		this->m22 * scalar,
		this->m23 * scalar,

		this->m31 * scalar,
		this->m32 * scalar,
		this->m33 * scalar
	);
}

Basis3D Basis3D::operator * (const Basis3D& other) const
{
	return Basis3D(
		(this->m11 * other.m11) + (this->m12 * other.m21) + (this->m13 * other.m31),
		(this->m11 * other.m12) + (this->m12 * other.m22) + (this->m13 * other.m32),
		(this->m11 * other.m13) + (this->m12 * other.m23) + (this->m13 * other.m33),

		(this->m21 * other.m11) + (this->m22 * other.m21) + (this->m23 * other.m31),
		(this->m21 * other.m12) + (this->m22 * other.m22) + (this->m23 * other.m32),
		(this->m21 * other.m13) + (this->m22 * other.m23) + (this->m23 * other.m33),

		(this->m31 * other.m11) + (this->m32 * other.m21) + (this->m33 * other.m31),
		(this->m31 * other.m12) + (this->m32 * other.m22) + (this->m33 * other.m32),
		(this->m31 * other.m13) + (this->m32 * other.m23) + (this->m33 * other.m33)
	);
}

Basis3D& Basis3D::operator -= (const Basis3D& other)
{
	(*this) = (*this) - other;
	return *this;
}

Basis3D& Basis3D::operator += (const Basis3D& other)
{
	(*this) = (*this) + other;
	return *this;
}

Basis3D& Basis3D::operator /= (float scalar)
{
	(*this) = (*this) / scalar;
	return *this;
}

Basis3D& Basis3D::operator *= (float scalar)
{
	(*this) = (*this) * scalar;
	return *this;
}

Basis3D& Basis3D::operator *= (const Basis3D& other)
{
	(*this) = (*this) * other;
	return *this;
}

const Basis3D& Basis3D::identity()
{
	static const Basis3D IDENTITY = Basis3D(
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	);

	return IDENTITY;
}

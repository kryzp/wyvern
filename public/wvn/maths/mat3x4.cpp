#include <wvn/maths/mat3x4.h>
#include <wvn/maths/quat.h>
#include <wvn/maths/vec3.h>

using namespace wvn;

Mat3x4::Mat3x4()
	: m11(0), m12(0), m13(0), m14(0)
	, m21(0), m22(0), m23(0), m24(0)
	, m31(0), m32(0), m33(0), m34(0)
{
}

Mat3x4::Mat3x4(const Mat3x4& other)
	: m11(other.m11), m12(other.m12), m13(other.m13), m14(other.m14)
	, m21(other.m21), m22(other.m22), m23(other.m23), m24(other.m24)
	, m31(other.m31), m32(other.m32), m33(other.m33), m34(other.m34)
{
}

Mat3x4::Mat3x4(float diag)
	: m11(diag), m12(0), m13(0), m14(0)
	, m21(0), m22(diag), m23(0), m24(0)
	, m31(0), m32(0), m33(diag), m34(0)
{
}

Mat3x4::Mat3x4(
	float m11, float m12, float m13, float m14,
	float m21, float m22, float m23, float m24,
	float m31, float m32, float m33, float m34
)
	: m11(m11), m12(m12), m13(m13), m14(m14)
	, m21(m21), m22(m22), m23(m23), m24(m24)
	, m31(m31), m32(m32), m33(m33), m34(m34)
{
}

Vec3F Mat3x4::offset(const Mat3x4& mat)
{
	return Vec3F(mat.m14, mat.m24, mat.m34);
}

Mat3x4 Mat3x4::create_skew(const Vec3F& amount)
{
	return Mat3x4(
		1, 0, amount.z, 0,
		0, amount.y, 0, 0,
		amount.x, 0, 1, 0
	);
}

Mat3x4 Mat3x4::create_scale(const Vec3F& scale)
{
	return Mat3x4(
		scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0
	);
}

Mat3x4 Mat3x4::create_scale(float x, float y, float z)
{
	return Mat3x4(
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0
	);
}

Mat3x4 Mat3x4::create_scale(float scale)
{
	return Mat3x4(
		scale, 0, 0, 0,
		0, scale, 0, 0,
		0, 0, scale, 0
	);
}

Mat3x4 Mat3x4::create_rotation(const Quat& quat)
{
	return Mat3x4(
		1.0f - 2.0f * (quat.y * quat.y + quat.z * quat.z),
		       2.0f * (quat.x * quat.y - quat.z * quat.w),
		       2.0f * (quat.x * quat.z + quat.y * quat.w),
		0.0f,

		       2.0f * (quat.x * quat.y + quat.z * quat.w),
		1.0f - 2.0f * (quat.x * quat.x + quat.z * quat.z),
		       2.0f * (quat.y * quat.z - quat.x * quat.w),
		0.0f,

		       2.0f * (quat.x * quat.z - quat.y * quat.w),
		       2.0f * (quat.y * quat.z + quat.x * quat.w),
		1.0f - 2.0f * (quat.x * quat.x + quat.y * quat.y),
		0.0f
	);
}

Mat3x4 Mat3x4::create_rotation(const Vec3F& axis, float angle)
{
	float sin_theta = CalcF::sin(angle);
	float cos_theta = CalcF::cos(angle);
	float cos_inv   = 1.0f - cos_theta;

	return Mat3x4(
		 (axis.x * axis.x * cos_inv) +           cos_theta,
		-(axis.x * axis.y * cos_inv) + (axis.z * sin_theta),
		 (axis.x * axis.z * cos_inv) + (axis.y * sin_theta),
		0.0f,

		-(axis.y * axis.x * cos_inv) - (axis.z * sin_theta),
		 (axis.y * axis.y * cos_inv) +           cos_theta,
		-(axis.y * axis.z * cos_inv) + (axis.x * sin_theta),
		0.0f,

		 (axis.z * axis.x * cos_inv) - (axis.y * sin_theta),
		-(axis.z * axis.y * cos_inv) - (axis.x * sin_theta),
		 (axis.z * axis.z * cos_inv) +           cos_theta,
		0.0f
	);
}

Mat3x4 Mat3x4::create_translation(const Vec3F& translation)
{
	return Mat3x4(
		1, 0, 0, translation.x,
		0, 1, 0, translation.y,
		0, 0, 1, translation.z
	);
}

Mat3x4 Mat3x4::create_translation(float x, float y, float z)
{
	return Mat3x4(
		1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z
	);
}

Mat3x4 Mat3x4::create_transform(
	const Vec3F& position,
	const Quat& quat,
	const Vec3F& scale,
	const Vec3F& origin
)
{
	Mat3x4 mat = Mat3x4::identity();

	if (origin != Vec3F::zero()) {
		mat *= create_translation(-origin);
	}

	if (scale != Vec3F::zero()) {
		mat *= create_scale(scale);
	}

	if (quat != Quat::zero()) {
		mat *= create_rotation(quat);
	}

	if (position != Vec3F::zero()) {
		mat *= create_translation(position);
	}

	return mat;
}

Vec3F Mat3x4::position() const
{
	return Vec3F(m31, m32, m33);
}

float Mat3x4::determinant() const
{
	float d1 = (m33 * m22) - (m32 * m23);
	float d2 = (m33 * m12) - (m32 * m13);
	float d3 = (m23 * m12) - (m22 * m13);

	return (m11 * d1) - (m21 * d2) + (m31 * d3);
}

Mat3x4 Mat3x4::inverse() const
{
	float det = determinant();

	return Mat3x4(
		((m22 * m33) - (m32 * m23)) / det,
		((m13 * m32) - (m12 * m33)) / det,
		((m12 * m23) - (m13 * m22)) / det,
		((m14 * ((m22 * m33) - (m32 * m23))) + (m24 * ((m23 * m31) - (m21 * m33))) + (m34 * ((m21 * m32) - (m31 * m22)))) / det,

		((m23 * m31) - (m21 * m33)) / det,
		((m11 * m33) - (m13 * m31)) / det,
		((m21 * m13) - (m11 * m23)) / det,
		((m14 * ((m13 * m32) - (m12 * m33))) + (m24 * ((m11 * m33) - (m13 * m31))) + (m34 * ((m31 * m12) - (m11 * m32)))) / det,

		((m21 * m32) - (m31 * m22)) / det,
		((m31 * m12) - (m11 * m32)) / det,
		((m11 * m22) - (m21 * m12)) / det,
		((m14 * ((m12 * m23) - (m13 * m22))) + (m24 * ((m21 * m13) - (m11 * m23))) + (m34 * ((m11 * m22) - (m21 * m12)))) / det
	);
}

Mat3x4 Mat3x4::operator - (const Mat3x4& other) const
{
	return Mat3x4(
		this->m11 - other.m11,
		this->m12 - other.m12,
		this->m13 - other.m13,
		this->m14 - other.m14,

		this->m21 - other.m21,
		this->m22 - other.m22,
		this->m23 - other.m23,
		this->m24 - other.m24,

		this->m31 - other.m31,
		this->m32 - other.m32,
		this->m33 - other.m33,
		this->m34 - other.m34
	);
}

Mat3x4 Mat3x4::operator + (const Mat3x4& other) const
{
	return Mat3x4(
		this->m11 + other.m11,
		this->m12 + other.m12,
		this->m13 + other.m13,
		this->m14 + other.m14,

		this->m21 + other.m21,
		this->m22 + other.m22,
		this->m23 + other.m23,
		this->m24 + other.m24,

		this->m31 + other.m31,
		this->m32 + other.m32,
		this->m33 + other.m33,
		this->m34 + other.m34
	);
}

Mat3x4 Mat3x4::operator * (const Mat3x4& other) const
{
	return Mat3x4(
		(this->m11 * other.m11) + (this->m12 * other.m21) + (this->m13 * other.m31),
		(this->m11 * other.m12) + (this->m12 * other.m22) + (this->m13 * other.m32),
		(this->m11 * other.m13) + (this->m12 * other.m23) + (this->m13 * other.m33),
		(this->m14 * other.m11) + (this->m24 * other.m21) + (this->m34 * other.m31) + other.m14,

		(this->m21 * other.m11) + (this->m22 * other.m21) + (this->m23 * other.m31),
		(this->m21 * other.m12) + (this->m22 * other.m22) + (this->m23 * other.m32),
		(this->m21 * other.m13) + (this->m22 * other.m23) + (this->m23 * other.m33),
		(this->m14 * other.m12) + (this->m24 * other.m22) + (this->m34 * other.m32) + other.m24,

		(this->m31 * other.m11) + (this->m32 * other.m21) + (this->m33 * other.m31),
		(this->m31 * other.m12) + (this->m32 * other.m22) + (this->m33 * other.m32),
		(this->m31 * other.m13) + (this->m32 * other.m23) + (this->m33 * other.m33),
		(this->m14 * other.m13) + (this->m24 * other.m23) + (this->m34 * other.m33) + other.m34
	);
}

Mat3x4 Mat3x4::operator * (float scalar) const
{
	return Mat3x4(
		this->m11 * scalar,
		this->m12 * scalar,
		this->m13 * scalar,
		this->m14 * scalar,

		this->m21 * scalar,
		this->m22 * scalar,
		this->m23 * scalar,
		this->m24 * scalar,

		this->m31 * scalar,
		this->m32 * scalar,
		this->m33 * scalar,
		this->m34 * scalar
	);
}

Mat3x4 Mat3x4::operator / (float scalar) const
{
	return Mat3x4(
		this->m11 / scalar,
		this->m12 / scalar,
		this->m13 / scalar,
		this->m14 / scalar,

		this->m21 / scalar,
		this->m22 / scalar,
		this->m23 / scalar,
		this->m24 / scalar,

		this->m31 / scalar,
		this->m32 / scalar,
		this->m33 / scalar,
		this->m34 / scalar
	);
}

Mat3x4& Mat3x4::operator -= (const Mat3x4& other)
{
	(*this) = (*this) - other;
	return *this;
}

Mat3x4& Mat3x4::operator += (const Mat3x4& other)
{
	(*this) = (*this) + other;
	return *this;
}

Mat3x4& Mat3x4::operator *= (float scalar)
{
	(*this) = (*this) * scalar;
	return *this;
}

Mat3x4& Mat3x4::operator *= (const Mat3x4& other)
{
	(*this) = (*this) * other;
	return *this;
}

Mat3x4& Mat3x4::operator /= (float scalar)
{
	(*this) = (*this) / scalar;
	return *this;
}

float* Mat3x4::operator [] (int idx)
{
	return elements[idx];
}

const float* Mat3x4::operator [] (int idx) const
{
	return elements[idx];
}

const Mat3x4& Mat3x4::identity()
{
	static const Mat3x4 IDENTITY = Mat3x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0
	);

	return IDENTITY;
}

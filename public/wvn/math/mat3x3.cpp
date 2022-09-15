#include <wvn/math/mat3x3.h>

using namespace wvn;

Mat3x3::Mat3x3()
	: m11(0), m12(0), m13(0)
	, m21(0), m22(0), m23(0)
	, m31(0), m32(0), m33(0)
{
}

Mat3x3::Mat3x3(const Mat3x3& other)
	: m11(other.m11), m12(other.m12), m13(other.m13)
	, m21(other.m21), m22(other.m22), m23(other.m23)
	, m31(other.m31), m32(other.m32), m33(other.m33)
{
}

Mat3x3::Mat3x3(float initial)
	: m11(initial), m12(0), m13(0)
	, m21(0), m22(initial), m23(0)
	, m31(0), m32(0), m33(initial)
{
}

Mat3x3::Mat3x3(
	float m11, float m12, float m13,
	float m21, float m22, float m23,
	float m31, float m32, float m33
)
	: m11(m11), m12(m12), m13(m13)
	, m21(m21), m22(m22), m23(m23)
	, m31(m31), m32(m32), m33(m33)
{
}

float* Mat3x3::value_ptr()
{
	return data;
}

const float* Mat3x3::value_ptr() const
{
	return data;
}

float Mat3x3::determinant() const
{
	float d1 = (m33 * m22) - (m32 * m23);
	float d2 = (m33 * m12) - (m32 * m13);
	float d3 = (m23 * m12) - (m22 * m13);

	return (m11 * d1) - (m21 * d2) + (m31 * d3);
}

Mat3x3 Mat3x3::inverse() const
{
	float det = determinant();

	return Mat3x3(
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

Mat3x3 Mat3x3::operator - (const Mat3x3& other) const
{
	return Mat3x3(
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

Mat3x3 Mat3x3::operator + (const Mat3x3& other) const
{
	return Mat3x3(
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

Mat3x3 Mat3x3::operator * (float scalar) const
{
	return Mat3x3(
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

Mat3x3 Mat3x3::operator * (const Mat3x3& other) const
{
	return Mat3x3(
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

Mat3x3& Mat3x3::operator -= (const Mat3x3& other)
{
	(*this) = (*this) - other;
	return *this;
}

Mat3x3& Mat3x3::operator += (const Mat3x3& other)
{
	(*this) = (*this) + other;
	return *this;
}

Mat3x3& Mat3x3::operator *= (float scalar)
{
	(*this) = (*this) * scalar;
	return *this;
}

Mat3x3& Mat3x3::operator *= (const Mat3x3& other)
{
	(*this) = (*this) * other;
	return *this;
}

float* Mat3x3::operator [] (int idx)
{
	return elements[idx];
}

const float* Mat3x3::operator [] (int idx) const
{
	return elements[idx];
}

const Mat3x3& Mat3x3::identity()
{
	static const Mat3x3 IDENTITY = Mat3x3(
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	);

	return IDENTITY;
}

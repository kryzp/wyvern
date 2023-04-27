#include <wvn/maths/mat4x3.h>
#include <wvn/maths/quaternion.h>
#include <wvn/maths/vec3.h>

using namespace wvn;

Mat4x3::Mat4x3()
	: m11(0), m12(0), m13(0)
	, m21(0), m22(0), m23(0)
	, m31(0), m32(0), m33(0)
	, m41(0), m42(0), m43(0)
{
}

Mat4x3::Mat4x3(const Mat4x3& other)
	: m11(other.m11), m12(other.m12), m13(other.m13)
	, m21(other.m21), m22(other.m22), m23(other.m23)
	, m31(other.m31), m32(other.m32), m33(other.m33)
	, m41(other.m41), m42(other.m42), m43(other.m43)
{
}

Mat4x3::Mat4x3(float diag)
	: m11(diag), m12(0), m13(0)
	, m21(0), m22(diag), m23(0)
	, m31(0), m32(0), m33(diag)
	, m41(0), m42(0), m43(0)
{
}

Mat4x3::Mat4x3(
	float m11, float m12, float m13,
	float m21, float m22, float m23,
	float m31, float m32, float m33,
	float m41, float m42, float m43
)
	: m11(m11), m12(m12), m13(m13)
	, m21(m21), m22(m22), m23(m23)
	, m31(m31), m32(m32), m33(m33)
	, m41(m41), m42(m42), m43(m43)
{
}

Vec3F Mat4x3::offset(const Mat4x3& mat)
{
	return Vec3F(mat.m41, mat.m42, mat.m43);
}

Mat4x3 Mat4x3::create_skew(const Vec3<float>& amount)
{
	return Mat4x3(
		1, 0, amount.z,
		0, amount.y, 0,
		amount.x, 0, 1,
		0, 0, 0
	);
}

Mat4x3 Mat4x3::create_scale(const Vec3<float>& scale)
{
	return Mat4x3(
		scale.x, 0, 0,
		0, scale.y, 0,
		0, 0, scale.z,
		0, 0, 0
	);
}

Mat4x3 Mat4x3::create_scale(float scale)
{
	return Mat4x3(
		scale, 0, 0,
		0, scale, 0,
		0, 0, scale,
		0, 0, 0
	);
}

Mat4x3 Mat4x3::create_rotation(const Quaternion& q)
{
	return Mat4x3(
		1.0f - (2.0f * ((q.s * q.s)) + (q.i * q.i)),
		2.0f * (        (q.i * q.j)  - (q.s * q.k)),
		2.0f * (        (q.i * q.k)  + (q.s * q.j)),

		2.0f * (        (q.i * q.j)  + (q.s * q.k)),
		1.0f - (2.0f * ((q.s * q.s)) + (q.j * q.j)),
		2.0f * (        (q.j * q.k)  - (q.s * q.i)),

		2.0f * (        (q.i * q.k)  - (q.s * q.j)),
		2.0f * (        (q.j * q.k)  + (q.s * q.i)),
		1.0f - (2.0f * ((q.s * q.s)) + (q.k * q.k)),

		0.0f,
		0.0f,
		0.0f
	);
}

Mat4x3 Mat4x3::create_translation(const Vec3<float>& translation)
{
	return Mat4x3(
		1, 0, 0,
		0, 1, 0,
		0, 0, 1,
		translation.x, translation.y, translation.z
	);
}

Mat4x3 Mat4x3::create_transform(
	const Vec3F& position,
	const Quaternion& quat,
	const Size3& scale,
	const Vec3F& origin
)
{
	Mat4x3 mat = Mat4x3::identity();

	if (origin != wvn::Vec3F::zero()) {
		mat *= create_translation(-origin);
	}

	if (scale != wvn::Size3::zero()) {
		mat *= create_scale(scale);
	}

	if (quat != Quaternion::zero()) {
		mat *= create_rotation(quat);
	}

	if (position != wvn::Vec3F::zero()) {
		mat *= create_translation(position);
	}

	return mat;
}

Vec3F Mat4x3::position() const
{
	return Vec3F(m31, m32, m33);
}

float Mat4x3::determinant() const
{
	float d1 = (m33 * m22) - (m32 * m23);
	float d2 = (m33 * m12) - (m32 * m13);
	float d3 = (m23 * m12) - (m22 * m13);

	return (m11 * d1) - (m21 * d2) + (m31 * d3);
}

Mat4x3 Mat4x3::inverse() const
{
	float det = determinant();

	return Mat4x3(
		((m22 * m33) - (m32 * m23)) / det,
		((m13 * m32) - (m12 * m33)) / det,
		((m12 * m23) - (m13 * m22)) / det,

		((m23 * m31) - (m21 * m33)) / det,
		((m11 * m33) - (m13 * m31)) / det,
		((m21 * m13) - (m11 * m23)) / det,

		((m21 * m32) - (m31 * m22)) / det,
		((m31 * m12) - (m11 * m32)) / det,
		((m11 * m22) - (m21 * m12)) / det,

		((m41 * ((m22 * m33) - (m32 * m23))) + (m42 * ((m23 * m31) - (m21 * m33))) + (m43 * ((m21 * m32) - (m31 * m22)))) / det,
		((m41 * ((m13 * m32) - (m12 * m33))) + (m42 * ((m11 * m33) - (m13 * m31))) + (m43 * ((m31 * m12) - (m11 * m32)))) / det,
		((m41 * ((m12 * m23) - (m13 * m22))) + (m42 * ((m21 * m13) - (m11 * m23))) + (m43 * ((m11 * m22) - (m21 * m12)))) / det
	);
}

Mat4x3 Mat4x3::operator - (const Mat4x3& other) const
{
	return Mat4x3(
		this->m11 - other.m11,
		this->m12 - other.m12,
		this->m13 - other.m13,

		this->m21 - other.m21,
		this->m22 - other.m22,
		this->m23 - other.m23,

		this->m31 - other.m31,
		this->m32 - other.m32,
		this->m33 - other.m33,

		this->m41 - other.m41,
		this->m42 - other.m42,
		this->m43 - other.m43
	);
}

Mat4x3 Mat4x3::operator + (const Mat4x3& other) const
{
	return Mat4x3(
		this->m11 + other.m11,
		this->m12 + other.m12,
		this->m13 + other.m13,

		this->m21 + other.m21,
		this->m22 + other.m22,
		this->m23 + other.m23,

		this->m31 + other.m31,
		this->m32 + other.m32,
		this->m33 + other.m33,

		this->m41 + other.m41,
		this->m42 + other.m42,
		this->m43 + other.m43
	);
}

Mat4x3 Mat4x3::operator * (float scalar) const
{
	return Mat4x3(
		this->m11 * scalar,
		this->m12 * scalar,
		this->m13 * scalar,

		this->m21 * scalar,
		this->m22 * scalar,
		this->m23 * scalar,

		this->m31 * scalar,
		this->m32 * scalar,
		this->m33 * scalar,

		this->m41 * scalar,
		this->m42 * scalar,
		this->m43 * scalar
	);
}

Mat4x3 Mat4x3::operator * (const Mat4x3& other) const
{
	return Mat4x3(
		(this->m11 * other.m11) + (this->m12 * other.m21) + (this->m13 * other.m31),
		(this->m11 * other.m12) + (this->m12 * other.m22) + (this->m13 * other.m32),
		(this->m11 * other.m13) + (this->m12 * other.m23) + (this->m13 * other.m33),

		(this->m21 * other.m11) + (this->m22 * other.m21) + (this->m23 * other.m31),
		(this->m21 * other.m12) + (this->m22 * other.m22) + (this->m23 * other.m32),
		(this->m21 * other.m13) + (this->m22 * other.m23) + (this->m23 * other.m33),

		(this->m31 * other.m11) + (this->m32 * other.m21) + (this->m33 * other.m31),
		(this->m31 * other.m12) + (this->m32 * other.m22) + (this->m33 * other.m32),
		(this->m31 * other.m13) + (this->m32 * other.m23) + (this->m33 * other.m33),

		(this->m41 * other.m11) + (this->m42 * other.m21) + (this->m43 * other.m31) + other.m41,
		(this->m41 * other.m12) + (this->m42 * other.m22) + (this->m43 * other.m32) + other.m42,
		(this->m41 * other.m13) + (this->m42 * other.m23) + (this->m43 * other.m33) + other.m43
	);
}

Mat4x3 Mat4x3::operator / (float scalar) const
{
	return Mat4x3(
		m11 / scalar,
		m12 / scalar,
		m13 / scalar,

		m21 / scalar,
		m22 / scalar,
		m23 / scalar,

		m31 / scalar,
		m32 / scalar,
		m33 / scalar,

		m41 / scalar,
		m42 / scalar,
		m43 / scalar
	);
}

Mat4x3& Mat4x3::operator -= (const Mat4x3& other)
{
	(*this) = (*this) - other;
	return *this;
}

Mat4x3& Mat4x3::operator += (const Mat4x3& other)
{
	(*this) = (*this) + other;
	return *this;
}

Mat4x3& Mat4x3::operator *= (float scalar)
{
	(*this) = (*this) * scalar;
	return *this;
}

Mat4x3& Mat4x3::operator *= (const Mat4x3& other)
{
	(*this) = (*this) * other;
	return *this;
}

Mat4x3& Mat4x3::operator /= (float scalar)
{
	(*this) = (*this) / scalar;
	return *this;
}

float* Mat4x3::operator [] (int idx)
{
	return elements[idx];
}

const float* Mat4x3::operator [] (int idx) const
{
	return elements[idx];
}

const Mat4x3& Mat4x3::identity()
{
	static const Mat4x3 IDENTITY = Mat4x3(
		1, 0, 0,
		0, 1, 0,
		0, 0, 1,
		0, 0, 0
	);

	return IDENTITY;
}

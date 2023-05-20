#include <wvn/maths/mat2x3.h>
#include <wvn/maths/vec2.h>
#include <wvn/maths/calc.h>

using namespace wvn;

Mat2x3::Mat2x3()
	: m11(0), m12(0), m13(0)
	, m21(0), m22(0), m23(0)
{
}

Mat2x3::Mat2x3(const Mat2x3& other)
	: m11(other.m11), m12(other.m12), m13(other.m13)
	, m21(other.m21), m22(other.m22), m23(other.m23)
{
}

Mat2x3::Mat2x3(float diag)
	: m11(diag), m12(0), m13(0)
	, m21(0), m22(diag), m23(0)
{
}

Mat2x3::Mat2x3(
	float m11, float m12, float m13,
	float m21, float m22, float m23
)
	: m11(m11), m12(m12), m13(m13)
	, m21(m21), m22(m22), m23(m23)
{
}

Vec2F Mat2x3::position() const
{
	return Vec2F(m13, m23);
}

float Mat2x3::determinant() const
{
	return (m11 * m22) - (m21 * m12);
}

Mat2x3 Mat2x3::inverse() const
{
	auto det = determinant();

	return Mat2x3(
		 m22 / det,
		-m12 / det,
		((m23 * m21) - (m13 * m22)) / det,

		-m21 / det,
		 m11 / det,
		((m13 * m12) - (m23 * m11)) / det
	);
}

Vec2F Mat2x3::offset(const Mat2x3& mat)
{
	return Vec2F(mat.m13, mat.m23);
}

Mat2x3 Mat2x3::create_skew(const Vec2F& amount)
{
	return Mat2x3(
		1, amount.y, 0,
		amount.x, 1, 0
	);
}

Mat2x3 Mat2x3::create_scale(const Vec2F& scale)
{
	return Mat2x3(
		scale.x, 0, 0,
		0, scale.y, 0
	);
}

Mat2x3 Mat2x3::create_scale(float scale)
{
	return Mat2x3(
		scale, 0, 0,
		0, scale, 0
	);
}

Mat2x3 Mat2x3::create_rotation(float angle)
{
	return Mat2x3(
		 CalcF::cos(angle), CalcF::sin(angle), 0,
		-CalcF::sin(angle), CalcF::cos(angle), 0
	);
}

Mat2x3 Mat2x3::create_translation(const Vec2F& translation)
{
	return Mat2x3(
		1, 0, translation.x,
		0, 1, translation.y
	);
}

Mat2x3 Mat2x3::create_transform(const Vec2F& position, float rotation, const Vec2F& scale, const Vec2F& origin)
{
	Mat2x3 mat = Mat2x3::identity();

	if (origin != Vec2F::zero()) {
		mat *= create_translation(-origin);
	}

	if (scale != Vec2F::one()) {
		mat *= create_scale(scale);
	}

	if (rotation != 0.0f) {
		mat *= create_rotation(rotation);
	}

	if (position != Vec2F::zero()) {
		mat *= create_translation(position);
	}

	return mat;
}

Mat2x3 Mat2x3::operator - (const Mat2x3& other) const
{
	return Mat2x3(
		m11 - other.m11,
		m12 - other.m12,
		m13 - other.m13,

		m21 - other.m21,
		m22 - other.m22,
		m23 - other.m23
	);
}

Mat2x3 Mat2x3::operator + (const Mat2x3& other) const
{
	return Mat2x3(
		m11 + other.m11,
		m12 + other.m12,
		m13 + other.m13,

		m21 + other.m21,
		m22 + other.m22,
		m23 + other.m23
	);
}

Mat2x3 Mat2x3::operator * (float scalar) const
{
	return Mat2x3(
		m11 * scalar,
		m12 * scalar,
		m13 * scalar,

		m21 * scalar,
		m22 * scalar,
		m23 * scalar
	);
}

Mat2x3 Mat2x3::operator * (const Mat2x3& other) const
{
	return Mat2x3(
		(this->m11 * other.m11) + (this->m12 * other.m21),
		(this->m11 * other.m12) + (this->m12 * other.m22),
		(this->m13 * other.m11) + (this->m23 * other.m21) + other.m13,

		(this->m21 * other.m11) + (this->m22 * other.m21),
		(this->m21 * other.m12) + (this->m22 * other.m22),
		(this->m13 * other.m12) + (this->m23 * other.m22) + other.m23
	);
}

Mat2x3& Mat2x3::operator -= (const Mat2x3& other)
{
	(*this) = (*this) - other;
	return *this;
}

Mat2x3& Mat2x3::operator += (const Mat2x3& other)
{
	(*this) = (*this) + other;
	return *this;
}

Mat2x3& Mat2x3::operator *= (float scalar)
{
	(*this) = (*this) * scalar;
	return *this;
}

Mat2x3& Mat2x3::operator *= (const Mat2x3& other)
{
	(*this) = (*this) * other;
	return *this;
}

float* Mat2x3::operator [] (int idx)
{
	return elements[idx];
}

const float* Mat2x3::operator [] (int idx) const
{
	return elements[idx];
}

const Mat2x3& Mat2x3::identity()
{
	static const Mat2x3 IDENTITY = Mat2x3(
		1, 0, 0,
		0, 1, 0
	);

	return IDENTITY;
}

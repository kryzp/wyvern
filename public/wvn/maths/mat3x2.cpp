#include <wvn/maths/mat3x2.h>
#include <wvn/maths/vec2.h>
#include <wvn/maths/calc.h>

// nice spelling of 'transformation' alan :/
// https://www.alanzucconi.com/2016/02/10/tranfsormation-matrix/

/* Here's how it works:
This is actually just a 2x2 transformation matrix + position (m31, m32)

So, multiplication means doing 2x2 matrix multiplication then also applying
that 'other' matrix to the position vector!
*/

using namespace wvn;

Mat3x2::Mat3x2()
	: m11(0), m12(0)
	, m21(0), m22(0)
	, m31(0), m32(0)
{
}

Mat3x2::Mat3x2(const Mat3x2& other)
	: m11(other.m11), m12(other.m12)
	, m21(other.m21), m22(other.m22)
	, m31(other.m31), m32(other.m32)
{
}

Mat3x2::Mat3x2(float initial)
	: m11(initial), m12(0)
	, m21(0),       m22(initial)
	, m31(0),       m32(0)
{
}

Mat3x2::Mat3x2(
	float m11, float m12,
	float m21, float m22,
	float m31, float m32
)
	: m11(m11), m12(m12)
	, m21(m21), m22(m22)
	, m31(m31), m32(m32)
{
}

Vec2F Mat3x2::position() const
{
	return Vec2F(m31, m32);
}

float* Mat3x2::value_ptr()
{
	return &m11;
}

const float* Mat3x2::value_ptr() const
{
    return &m11;
}

float Mat3x2::determinant() const
{
	return (m11 * m22) - (m21 * m12);
}

Mat3x2 Mat3x2::inverse() const
{
	auto det = determinant();

	return Mat3x2(
		 m22 / det,
		-m12 / det,
		-m21 / det,
		 m11 / det,
		((m32 * m21) - (m31 * m22)) / det,
		((m31 * m12) - (m32 * m11)) / det
	);
}

Vec2F Mat3x2::offset(const Mat3x2& mat)
{
	return Vec2F(mat.m31, mat.m32);
}

Mat3x2 Mat3x2::create_skew(const Vec2F& amount)
{
	return Mat3x2(
		1, amount.y,
		amount.x, 1,
		0, 0
	);
}

Mat3x2 Mat3x2::create_scale(const Vec2F& scale)
{
	return Mat3x2(
		scale.x, 0,
		0, scale.y,
		0, 0
	);
}

Mat3x2 Mat3x2::create_scale(float scale)
{
	return Mat3x2(
		scale, 0,
		0, scale,
		0, 0
	);
}

Mat3x2 Mat3x2::create_rotation(float rotation)
{
	return Mat3x2(
		CalcF::cos(rotation), CalcF::sin(rotation),
		-CalcF::sin(rotation), CalcF::cos(rotation),
		0, 0
	);
}

Mat3x2 Mat3x2::create_translation(const Vec2F& translation)
{
	return Mat3x2(
		1, 0,
		0, 1,
		translation.x, translation.y
	);
}

Mat3x2 Mat3x2::create_transform(const Vec2F& position, float rotation, const Vec2F& scale, const Vec2F& origin)
{
	Mat3x2 mat = Mat3x2::identity();

	if (origin != wvn::Vec2F::zero())
		mat *= create_translation(-origin);

	if (scale != wvn::Vec2F::one())
		mat *= create_scale(scale);

	if (rotation != 0.0f)
		mat *= create_rotation(rotation);

	if (position != wvn::Vec2F::zero())
		mat *= create_translation(position);

	return mat;
}

Mat3x2 Mat3x2::operator - (const Mat3x2& other) const
{
	return Mat3x2(
		m11 - other.m11,
		m12 - other.m12,

		m21 - other.m21,
		m22 - other.m22,

		m31 - other.m31,
		m32 - other.m32
	);
}

Mat3x2 Mat3x2::operator + (const Mat3x2& other) const
{
	return Mat3x2(
		m11 + other.m11,
		m12 + other.m12,

		m21 + other.m21,
		m22 + other.m22,

		m31 + other.m31,
		m32 + other.m32
	);
}

Mat3x2 Mat3x2::operator * (float scalar) const
{
	return Mat3x2(
		m11 * scalar,
		m12 * scalar,

		m21 * scalar,
		m22 * scalar,

		m31 * scalar,
		m32 * scalar
	);
}

Mat3x2 Mat3x2::operator * (const Mat3x2& other) const
{
	return Mat3x2(
		(this->m11 * other.m11) + (this->m12 * other.m21),
		(this->m11 * other.m12) + (this->m12 * other.m22),

		(this->m21 * other.m11) + (this->m22 * other.m21),
		(this->m21 * other.m12) + (this->m22 * other.m22),

		(this->m31 * other.m11) + (this->m32 * other.m21) + other.m31, 
		(this->m31 * other.m12) + (this->m32 * other.m22) + other.m32
	);
}

Mat3x2& Mat3x2::operator -= (const Mat3x2& other)
{
	(*this) = (*this) - other;
	return *this;
}

Mat3x2& Mat3x2::operator += (const Mat3x2& other)
{
	(*this) = (*this) + other;
	return *this;
}

Mat3x2& Mat3x2::operator *= (float scalar)
{
	(*this) = (*this) * scalar;
	return *this;
}

Mat3x2& Mat3x2::operator *= (const Mat3x2& other)
{
	(*this) = (*this) * other;
	return *this;
}

float* Mat3x2::operator [] (int idx)
{
	return elements[idx];
}

const float* Mat3x2::operator [] (int idx) const
{
	return elements[idx];
}

const Mat3x2& Mat3x2::identity()
{
	static const Mat3x2 IDENTITY = Mat3x2(
		1, 0,
		0, 1,
		0, 0
	);

	return IDENTITY;
}

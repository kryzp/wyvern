#include <wvn/maths/mat4x4.h>
#include <wvn/maths/mat3x2.h>
#include <wvn/maths/mat4x3.h>
#include <wvn/maths/vec3.h>
#include <wvn/maths/calc.h>

#include <wvn/util/assert.h>

using namespace wvn;

Mat4x4::Mat4x4()
	: m11(0), m12(0), m13(0), m14(0)
	, m21(0), m22(0), m23(0), m24(0)
	, m31(0), m32(0), m33(0), m34(0)
	, m41(0), m42(0), m43(0), m44(0)
{
}

Mat4x4::Mat4x4(const Mat4x4& other)
	: m11(other.m11), m12(other.m12), m13(other.m13), m14(other.m14)
	, m21(other.m21), m22(other.m22), m23(other.m23), m24(other.m24)
	, m31(other.m31), m32(other.m32), m33(other.m33), m34(other.m34)
	, m41(other.m41), m42(other.m42), m43(other.m43), m44(other.m44)
{
}

Mat4x4::Mat4x4(float initial)
	: m11(initial), m12(0), m13(0), m14(0)
	, m21(0), m22(initial), m23(0), m24(0)
	, m31(0), m32(0), m33(initial), m34(0)
	, m41(0), m42(0), m43(0), m44(initial)
{
}

Mat4x4::Mat4x4(
	float m11, float m12, float m13, float m14,
	float m21, float m22, float m23, float m24,
	float m31, float m32, float m33, float m34,
	float m41, float m42, float m43, float m44
)
	: m11(m11), m12(m12), m13(m13), m14(m14)
	, m21(m21), m22(m22), m23(m23), m24(m24)
	, m31(m31), m32(m32), m33(m33), m34(m34)
	, m41(m41), m42(m42), m43(m43), m44(m44)
{
}

float* Mat4x4::value_ptr()
{
	return data;
}

const float* Mat4x4::value_ptr() const
{
	return data;
}

Mat4x4 Mat4x4::create_orthographic(float width, float height, float near, float far)
{
	return create_orthographic_ext(0.0f, width, height, 0.0f, near, far);
}

Mat4x4 Mat4x4::create_orthographic_ext(float left, float right, float bottom, float top, float near, float far)
{
	Mat4x4 result = Mat4x4::identity();

	result.m11 = 2.0f / (right - left);

	result.m22 = 2.0f / (top - bottom);

	result.m33 = 1.0f / (near - far);

	result.m41 = (left + right) / (left - right);
	result.m42 = (top + bottom) / (bottom - top);
	result.m43 = near / (near - far);
	result.m44 = 1.0f;

	return result;
}

Mat4x4 Mat4x4::create_perspective(float fov, float aspect, float near, float far)
{
    WVN_ASSERT(fov > 0, "[MAT4X4:DEBUG] FOV must be greater than 0.");
	WVN_ASSERT(aspect != 0, "[MAT4X4:DEBUG] Aspect must not be 0.");

    Mat4x4 result = Mat4x4(0.0f);

	const float tan_half_fov = CalcF::tan(fov / 2.0f);

	result.m11 = 1.0f / (aspect * tan_half_fov);
	result.m22 = 1.0f / (tan_half_fov);
	result.m33 = - (far + near) / (far - near);
	result.m34 = -1.0f;
	result.m43 = -(2.0f * far * near) / (far - near);

	return result;
}

Mat4x4 Mat4x4::from_mat3x2(const Mat3x2& mat)
{
	return Mat4x4(
		mat.m11, mat.m12, 0.0f, 0.0f,
		mat.m21, mat.m22, 0.0f, 0.0f,
		0.0f,    0.0f,    1.0f, 0.0f,
		mat.m31, mat.m32, 0.0f, 1.0f
	);
}

Mat4x4 from_mat4x4(const Mat4x3& mat)
{
	return Mat4x4(
		mat.m11, mat.m12, mat.m13, 0.0f,
		mat.m21, mat.m22, mat.m23, 0.0f,
		mat.m31, mat.m32, mat.m33, 0.0f,
		mat.m41, mat.m42, mat.m43, 1.0f
	);
}

Mat4x4 Mat4x4::create_translation(float x, float y, float z)
{
	return Mat4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1
	);
}

Mat4x4 Mat4x4::create_rotation(float angle, Vec3F axis)
{
	axis = axis.normalized();

	float sin_theta = CalcF::sin(angle);
	float cos_theta = CalcF::cos(angle);
	float cos_inv   = 1.0f - cos_theta;

	return Mat4x4(
		(axis.x * axis.x * cos_inv) + cos_theta,
		(axis.x * axis.y * cos_inv) + (axis.z * sin_theta),
		(axis.x * axis.z * cos_inv) - (axis.y * sin_theta),
		0,

		(axis.y * axis.x * cos_inv) - (axis.z * sin_theta),
		(axis.y * axis.y * cos_inv) + cos_theta,
		(axis.y * axis.z * cos_inv) + (axis.x * sin_theta),
		0,

		(axis.z * axis.x * cos_inv) + (axis.y * sin_theta),
		(axis.z * axis.y * cos_inv) - (axis.x * sin_theta),
		(axis.z * axis.z * cos_inv) + cos_theta,
		0,

		0,
		0,
		0,
		1
	);
}

Mat4x4 Mat4x4::create_scale(float x, float y, float z)
{
	return Mat4x4(
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1
	);
}

Mat4x4 Mat4x4::create_lookat(const Vec3F& eye, const Vec3F& centre, const Vec3F& up)
{
	Vec3F p = (centre - eye).normalized();
	Vec3F q = Vec3F::cross(p, up).normalized();
	Vec3F r = Vec3F::cross(q, p);

	return Mat4x4(
		q.x, r.x, -p.x, 0.0f,
		q.y, r.y, -p.y, 0.0f,
		q.z, r.z, -p.z, 0.0f,
		-Vec3F::dot(q, eye), -Vec3F::dot(r, eye), Vec3F::dot(p, eye), 1.0f
	);
}

float Mat4x4::determinant() const
{
    // yoink
    //https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix

    float A2323 = (this->m33 * this->m44) - (this->m34 * this->m43);
    float A1323 = (this->m32 * this->m44) - (this->m34 * this->m42);
    float A1223 = (this->m32 * this->m43) - (this->m33 * this->m42);
    float A0323 = (this->m31 * this->m44) - (this->m34 * this->m41);
    float A0223 = (this->m31 * this->m43) - (this->m33 * this->m41);
    float A0123 = (this->m31 * this->m42) - (this->m32 * this->m41);

    return (
        this->m11 * ((this->m22 * A2323) - (this->m23 * A1323) + (this->m24 * A1223)) -
        this->m12 * ((this->m21 * A2323) - (this->m23 * A0323) + (this->m24 * A0223)) +
        this->m13 * ((this->m21 * A1323) - (this->m22 * A0323) + (this->m24 * A0123)) -
        this->m14 * ((this->m21 * A1223) - (this->m22 * A0223) + (this->m23 * A0123))
    );
}

Mat4x4 Mat4x4::inverse() const
{
    // yoink
    //https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix

    float A2323 = (this->m33 * this->m44) - (this->m34 * this->m43);
    float A1323 = (this->m32 * this->m44) - (this->m34 * this->m42);
    float A1223 = (this->m32 * this->m43) - (this->m33 * this->m42);
    float A0323 = (this->m31 * this->m44) - (this->m34 * this->m41);
    float A0223 = (this->m31 * this->m43) - (this->m33 * this->m41);
    float A0123 = (this->m31 * this->m42) - (this->m32 * this->m41);
    float A2313 = (this->m23 * this->m44) - (this->m24 * this->m43);
    float A1313 = (this->m22 * this->m44) - (this->m24 * this->m42);
    float A1213 = (this->m22 * this->m43) - (this->m23 * this->m42);
    float A2312 = (this->m23 * this->m34) - (this->m24 * this->m33);
    float A1312 = (this->m22 * this->m34) - (this->m24 * this->m32);
    float A1212 = (this->m22 * this->m33) - (this->m23 * this->m32);
    float A0313 = (this->m21 * this->m44) - (this->m24 * this->m41);
    float A0213 = (this->m21 * this->m43) - (this->m23 * this->m41);
    float A0312 = (this->m21 * this->m34) - (this->m24 * this->m31);
    float A0212 = (this->m21 * this->m33) - (this->m23 * this->m31);
    float A0113 = (this->m21 * this->m42) - (this->m22 * this->m41);
    float A0112 = (this->m21 * this->m32) - (this->m22 * this->m31);

    float det =
        this->m11 * ((this->m22 * A2323) - (this->m23 * A1323) + (this->m24 * A1223)) -
        this->m12 * ((this->m21 * A2323) - (this->m23 * A0323) + (this->m24 * A0223)) +
        this->m13 * ((this->m21 * A1323) - (this->m22 * A0323) + (this->m24 * A0123)) -
        this->m14 * ((this->m21 * A1223) - (this->m22 * A0223) + (this->m23 * A0123));

    det = 1.0f / det;

    return Mat4x4(
        det *   ((this->m22 * A2323) - (this->m23 * A1323) + (this->m24 * A1223)),
        det * - ((this->m12 * A2323) - (this->m13 * A1323) + (this->m14 * A1223)),
        det *   ((this->m12 * A2313) - (this->m13 * A1313) + (this->m14 * A1213)),
        det * - ((this->m12 * A2312) - (this->m13 * A1312) + (this->m14 * A1212)),
        det * - ((this->m21 * A2323) - (this->m23 * A0323) + (this->m24 * A0223)),
        det *   ((this->m11 * A2323) - (this->m13 * A0323) + (this->m14 * A0223)),
        det * - ((this->m11 * A2313) - (this->m13 * A0313) + (this->m14 * A0213)),
        det *   ((this->m11 * A2312) - (this->m13 * A0312) + (this->m14 * A0212)),
        det *   ((this->m21 * A1323) - (this->m22 * A0323) + (this->m24 * A0123)),
        det * - ((this->m11 * A1323) - (this->m12 * A0323) + (this->m14 * A0123)),
        det *   ((this->m11 * A1313) - (this->m12 * A0313) + (this->m14 * A0113)),
        det * - ((this->m11 * A1312) - (this->m12 * A0312) + (this->m14 * A0112)),
        det * - ((this->m21 * A1223) - (this->m22 * A0223) + (this->m23 * A0123)),
        det *   ((this->m11 * A1223) - (this->m12 * A0223) + (this->m13 * A0123)),
        det * - ((this->m11 * A1213) - (this->m12 * A0213) + (this->m13 * A0113)),
        det *   ((this->m11 * A1212) - (this->m12 * A0212) + (this->m13 * A0112))
    );
}

Mat4x4 Mat4x4::operator - (const Mat4x4& other) const
{
	return Mat4x4(
		m11 - other.m11,
		m12 - other.m12,
		m13 - other.m13,
		m14 - other.m14,

		m21 - other.m21,
		m22 - other.m22,
		m23 - other.m23,
		m24 - other.m24,

		m31 - other.m31,
		m32 - other.m32,
		m33 - other.m33,
		m34 - other.m34,

		m41 - other.m41,
		m42 - other.m42,
		m43 - other.m43,
		m44 - other.m44
	);
}

Mat4x4 Mat4x4::operator + (const Mat4x4& other) const
{
	return Mat4x4(
		m11 + other.m11,
		m12 + other.m12,
		m13 + other.m13,
		m14 + other.m14,

		m21 + other.m21,
		m22 + other.m22,
		m23 + other.m23,
		m24 + other.m24,

		m31 + other.m31,
		m32 + other.m32,
		m33 + other.m33,
		m34 + other.m34,

		m41 + other.m41,
		m42 + other.m42,
		m43 + other.m43,
		m44 + other.m44
	);
}

Mat4x4 Mat4x4::operator * (const Mat4x4& other) const
{
	return Mat4x4(
		(this->m11 * other.m11) + (this->m12 * other.m21) + (this->m13 * other.m31) + (this->m14 * other.m41),
		(this->m11 * other.m12) + (this->m12 * other.m22) + (this->m13 * other.m32) + (this->m14 * other.m42),
		(this->m11 * other.m13) + (this->m12 * other.m23) + (this->m13 * other.m33) + (this->m14 * other.m43),
		(this->m11 * other.m14) + (this->m12 * other.m24) + (this->m13 * other.m34) + (this->m14 * other.m44),

		(this->m21 * other.m11) + (this->m22 * other.m21) + (this->m23 * other.m31) + (this->m24 * other.m41),
		(this->m21 * other.m12) + (this->m22 * other.m22) + (this->m23 * other.m32) + (this->m24 * other.m42),
		(this->m21 * other.m13) + (this->m22 * other.m23) + (this->m23 * other.m33) + (this->m24 * other.m43),
		(this->m21 * other.m14) + (this->m22 * other.m24) + (this->m23 * other.m34) + (this->m24 * other.m44),

		(this->m31 * other.m11) + (this->m32 * other.m21) + (this->m33 * other.m31) + (this->m34 * other.m41),
		(this->m31 * other.m12) + (this->m32 * other.m22) + (this->m33 * other.m32) + (this->m34 * other.m42),
		(this->m31 * other.m13) + (this->m32 * other.m23) + (this->m33 * other.m33) + (this->m34 * other.m43),
		(this->m31 * other.m14) + (this->m32 * other.m24) + (this->m33 * other.m34) + (this->m34 * other.m44),

		(this->m41 * other.m11) + (this->m42 * other.m21) + (this->m43 * other.m31) + (this->m44 * other.m41),
		(this->m41 * other.m12) + (this->m42 * other.m22) + (this->m43 * other.m32) + (this->m44 * other.m42),
		(this->m41 * other.m13) + (this->m42 * other.m23) + (this->m43 * other.m33) + (this->m44 * other.m43),
		(this->m41 * other.m14) + (this->m42 * other.m24) + (this->m43 * other.m34) + (this->m44 * other.m44)
	);
}

Mat4x4& Mat4x4::operator -= (const Mat4x4& other)
{
	*this = (*this) - other;
	return *this;
}

Mat4x4& Mat4x4::operator += (const Mat4x4& other)
{
	*this = (*this) + other;
	return *this;
}

Mat4x4& Mat4x4::operator *= (const Mat4x4& other)
{
	*this = (*this) * other;
	return *this;
}

float* Mat4x4::operator [] (int idx)
{
	return elements[idx];
}

const Mat4x4& Mat4x4::identity()
{
	static const Mat4x4 IDENTITY = Mat4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);

	return IDENTITY;
}

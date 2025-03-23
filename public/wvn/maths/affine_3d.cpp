#include <wvn/maths/affine_3d.h>
#include <wvn/maths/mat4x4.h>
#include <wvn/maths/quat.h>
#include <wvn/maths/vec3.h>

using namespace wvn;

Affine3D::Affine3D()
	: basis(Basis3D::identity())
	, origin(0.0f, 0.0f, 0.0f)
{
}

Affine3D::Affine3D(const Affine3D& other)
	: basis(other.basis)
	, origin(other.origin)
{
}

Affine3D::Affine3D(float diag)
	: basis(diag)
	, origin(0.0f, 0.0f, 0.0f)
{
}

Affine3D::Affine3D(const Basis3D& basis, const Vec3F& origin)
	: basis(basis)
	, origin(origin)
{
}

Vec3F Affine3D::transform(const Vec3F& vec, const Affine3D& mat)
{
	return Basis3D::transform(vec, mat.basis) + mat.origin;
}

Mat4x4 Affine3D::build_transformation_matrix() const
{
	return Mat4x4(
		basis.m11, basis.m12, basis.m13, origin.x,
		basis.m21, basis.m22, basis.m23, origin.y,
		basis.m31, basis.m32, basis.m33, origin.z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

Affine3D Affine3D::create_skew(const Vec3F& skew)
{
	return Affine3D(
		Basis3D::create_skew(skew),
		Vec3F::zero()
	);
}

Affine3D Affine3D::create_scale(const Vec3F& scale)
{
	return Affine3D(
		Basis3D::create_scale(scale),
		Vec3F::zero()
	);
}

Affine3D Affine3D::create_scale(float x, float y, float z)
{
	return Affine3D(
		Basis3D::create_scale(x, y, z),
		Vec3F::zero()
	);
}

Affine3D Affine3D::create_scale(float scale)
{
	return Affine3D(
		Basis3D::create_scale(scale),
		Vec3F::zero()
	);
}

Affine3D Affine3D::create_rotation(const Quat& quat)
{
	return Affine3D(
		Basis3D::create_rotation(quat),
		Vec3F::zero()
	);
}

Affine3D Affine3D::create_rotation(const Vec3F& axis, float angle)
{
	return Affine3D(
		Basis3D::create_rotation(axis, angle),
		Vec3F::zero()
	);
}

Affine3D Affine3D::create_translation(const Vec3F& translation)
{
	return Affine3D(
		Basis3D::identity(),
		translation
	);
}

Affine3D Affine3D::create_translation(float x, float y, float z)
{
	return Affine3D(
		Basis3D::identity(),
		Vec3F(x, y, z)
	);
}

Affine3D Affine3D::create_transform(
	const Vec3F& position,
	const Quat& quat,
	const Vec3F& scale,
	const Vec3F& origin
)
{
	Affine3D mat = Affine3D::identity();

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

float Affine3D::determinant() const
{
	return basis.determinant();
}

Affine3D Affine3D::inverse() const
{
	Basis3D inv = basis.inverse();

	return Affine3D(
		inv,
		Basis3D::transform(origin, inv)
	);
}

Affine3D Affine3D::operator - (const Affine3D& other) const
{
	return Affine3D(
		this->basis - other.basis,
		this->origin - other.origin
	);
}

Affine3D Affine3D::operator + (const Affine3D& other) const
{
	return Affine3D(
		this->basis + other.basis,
		this->origin + other.origin
	);
}

Affine3D Affine3D::operator * (const Affine3D& other) const
{
	return Affine3D(
		this->basis * other.basis,
		Basis3D::transform(origin, other.basis) + other.origin
	);
}

Affine3D Affine3D::operator * (float scalar) const
{
	return Affine3D(
		this->basis * scalar,
		this->origin * scalar
	);
}

Affine3D Affine3D::operator / (float scalar) const
{
	return Affine3D(
		this->basis / scalar,
		this->origin / scalar
	);
}

Affine3D& Affine3D::operator -= (const Affine3D& other)
{
	(*this) = (*this) - other;
	return *this;
}

Affine3D& Affine3D::operator += (const Affine3D& other)
{
	(*this) = (*this) + other;
	return *this;
}

Affine3D& Affine3D::operator *= (float scalar)
{
	(*this) = (*this) * scalar;
	return *this;
}

Affine3D& Affine3D::operator *= (const Affine3D& other)
{
	(*this) = (*this) * other;
	return *this;
}

Affine3D& Affine3D::operator /= (float scalar)
{
	(*this) = (*this) / scalar;
	return *this;
}

const Affine3D& Affine3D::identity()
{
	static const Affine3D IDENTITY = Affine3D(
		Basis3D::identity(),
		Vec3F::zero()
	);

	return IDENTITY;
}

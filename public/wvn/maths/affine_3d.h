#ifndef MAT3X4_H_
#define MAT3X4_H_

#include <wvn/maths/vec3.h>
#include <wvn/maths/basis_3d.h>

namespace wvn
{
	struct Quat;
	struct Mat4x4;

	/**
	 * Affine 3D transformation matrix.
	 */
	struct Affine3D
	{
		Basis3D basis;
		Vec3F origin;

		Affine3D();
		Affine3D(const Affine3D& other);
		Affine3D(float diag);
		Affine3D(const Basis3D& basis, const Vec3F& origin);

		static const Affine3D& identity();

		static Vec3F transform(const Vec3F& vec, const Affine3D& mat);
		Mat4x4 build_transformation_matrix() const;

		static Affine3D create_skew(const Vec3F& skew);
		static Affine3D create_scale(const Vec3F& scale);
		static Affine3D create_scale(float x, float y, float z);
		static Affine3D create_scale(float scale);
		static Affine3D create_rotation(const Quat& quat);
		static Affine3D create_rotation(const Vec3F& axis, float angle);
		static Affine3D create_translation(const Vec3F& translation);
		static Affine3D create_translation(float x, float y, float z);
		static Affine3D create_transform(const Vec3F& position, const Quat& rotation, const Vec3F& scale, const Vec3F& origin);

		float determinant() const;
		Affine3D inverse() const;

		Affine3D operator - (const Affine3D& other) const;
		Affine3D operator + (const Affine3D& other) const;
		Affine3D operator * (float scalar         ) const;
		Affine3D operator * (const Affine3D& other) const;
		Affine3D operator / (float scalar         ) const;

		Affine3D& operator -= (const Affine3D& other);
		Affine3D& operator += (const Affine3D& other);
		Affine3D& operator *= (float scalar         );
		Affine3D& operator *= (const Affine3D& other);
		Affine3D& operator /= (float scalar         );
	};
}

#endif // MAT3X4_H_

#ifndef MAT4X4_H_
#define MAT4X4_H_

#include <wvn/maths/vec3.h>
#include <wvn/maths/quat.h>

namespace wvn
{
	struct Mat2x3;
	struct Affine3D;

	/**
	 * 4x4 general purpose matrix.
	 */
	struct Mat4x4
	{
		union
		{
			struct
			{
				float m11, m21, m31, m41;
				float m12, m22, m32, m42;
				float m13, m23, m33, m43;
				float m14, m24, m34, m44;
			};

			float elements[4][4];
			float data[16];
		};

		Mat4x4();
		Mat4x4(const Mat4x4& other);
		Mat4x4(float diag);
		Mat4x4(
			float m11, float m12, float m13, float m14,
			float m21, float m22, float m23, float m24,
			float m31, float m32, float m33, float m34,
			float m41, float m42, float m43, float m44
		);

		static const Mat4x4& zero();
		static const Mat4x4& identity();

		static Mat4x4 create_translation(float x, float y, float z);
		static Mat4x4 create_rotation(Vec3F axis, float angle);
		static Mat4x4 create_rotation(const Quat& quat);
		static Mat4x4 create_scale(float x, float y, float z);

		static Mat4x4 create_lookat(const Vec3F& eye, const Vec3F& centre, const Vec3F& up);

		static Mat4x4 create_orthographic(float width, float height, float near, float far);
		static Mat4x4 create_orthographic_ext(float left, float right, float bottom, float top, float near, float far);

        static Mat4x4 create_perspective(float fov, float aspect, float near, float far);

		Mat4x4 basis() const;

        float determinant() const;
        Mat4x4 inverse() const;

		Mat4x4 operator - (const Mat4x4& other) const;
		Mat4x4 operator + (const Mat4x4& other) const;
		Mat4x4 operator * (const Mat4x4& other) const;

		Mat4x4& operator -= (const Mat4x4& other);
		Mat4x4& operator += (const Mat4x4& other);
		Mat4x4& operator *= (const Mat4x4& other);
	};
};

#endif // MAT4X4_H_

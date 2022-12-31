#pragma once

#include <wvn/maths/vec3.h>

namespace wvn
{
	struct Mat3x2;
	struct Mat4x3;

	/*
	 * Generic 4x4 matrix.
	 * Useful for camera / projection based transformations.
	 */
	struct Mat4x4
	{
		union
		{
			struct
			{
				float m11, m12, m13, m14;
				float m21, m22, m23, m24;
				float m31, m32, m33, m34;
				float m41, m42, m43, m44;
			};

			float elements[4][4];
			float data[16];
		};

		Mat4x4();
		Mat4x4(const Mat4x4& other);
		Mat4x4(float initial);
		Mat4x4(
			float m11, float m12, float m13, float m14,
			float m21, float m22, float m23, float m24,
			float m31, float m32, float m33, float m34,
			float m41, float m42, float m43, float m44
		);

		static const Mat4x4& identity();

		static Mat4x4 from_mat3x2(const Mat3x2& mat);
		static Mat4x4 from_mat4x3(const Mat4x3& mat);

		static Mat4x4 create_translation(float x, float y, float z);
		static Mat4x4 create_rotation(float angle, Vec3F axis);
		static Mat4x4 create_scale(float x, float y, float z);

		static Mat4x4 create_lookat(const Vec3F& eye, const Vec3F& centre, const Vec3F& up);

		static Mat4x4 create_orthographic(float width, float height, float near, float far);
		static Mat4x4 create_orthographic_ext(float left, float right, float bottom, float top, float near, float far);

        static Mat4x4 create_perspective(float fov, float aspect, float near, float far);

		float* value_ptr();
        const float* value_ptr() const;

        float determinant() const;
        Mat4x4 inverse() const;

		Mat4x4 operator - (const Mat4x4& other) const;
		Mat4x4 operator + (const Mat4x4& other) const;
		Mat4x4 operator * (const Mat4x4& other) const;

		Mat4x4& operator -= (const Mat4x4& other);
		Mat4x4& operator += (const Mat4x4& other);
		Mat4x4& operator *= (const Mat4x4& other);

		float* operator [] (int idx);
	};
};

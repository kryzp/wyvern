#pragma once

// TODO:
// OK THIS IS GETTING OUT OF HAND
// template based Matrix<R, C> structure?

namespace wvn
{
	template <typename T> struct Vec3;
	struct Quaternion;

	struct Mat4x3
	{
		union
		{
			struct
			{
				float m11;
				float m12;
				float m13;
				float m21;
				float m22;
				float m23;
				float m31;
				float m32;
				float m33;
				float m41;
				float m42;
				float m43;
			};

			float elements[4][3];
		};

		Mat4x3();
		Mat4x3(const Mat4x3& other);
		Mat4x3(
			float m11, float m12, float m13,
			float m21, float m22, float m23,
			float m31, float m32, float m33,
			float m41, float m42, float m43
		);

		static const Mat4x3& identity();

		static Vec3<float> offset(const Mat4x3& mat);

		static Mat4x3 create_skew(const Vec3<float>& amount);
		static Mat4x3 create_scale(const Vec3<float>& scale);
		static Mat4x3 create_scale(float scale);
		static Mat4x3 create_rotation(const Quaternion& quat);
		static Mat4x3 create_translation(const Vec3<float>& translation);
		static Mat4x3 create_transform(const Vec3<float>& position, const Quaternion& rotation, const Vec3<float>& scale, const Vec3<float>& origin);

		float* value_ptr();
		const float* value_ptr() const;

		float determinant() const;
		Mat4x3 inverse() const;

		Mat4x3 operator - (const Mat4x3& other) const;
		Mat4x3 operator + (const Mat4x3& other) const;
		Mat4x3 operator * (const Mat4x3& other) const;

		Mat4x3& operator -= (const Mat4x3& other);
		Mat4x3& operator += (const Mat4x3& other);
		Mat4x3& operator *= (const Mat4x3& other);

		float* operator [] (int idx);
		const float* operator [] (int idx) const;
	};
}

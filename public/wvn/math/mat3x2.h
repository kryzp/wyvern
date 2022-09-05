#pragma once

namespace wvn
{
	template <typename T> struct Vec2;
	struct Mat4x4;

	// special matrix used for 2D transformations
	// mathematically this doesnt actually make sense

	struct Mat3x2
	{
		union
		{
			struct
			{
				float m11;
				float m12;
				float m21;
				float m22;
				float m31;
				float m32;
			};

			float elements[3][2];
		};

		Mat3x2();
		Mat3x2(const Mat3x2& other);
		Mat3x2(float initial);
		Mat3x2(
			float m11, float m12,
			float m21, float m22,
			float m31, float m32
		);

		static const Mat3x2& identity();

		static Vec2<float> offset(const Mat3x2& mat);

		static Mat3x2 create_skew(const Vec2<float>& amount);
		static Mat3x2 create_scale(const Vec2<float>& scale);
		static Mat3x2 create_scale(float scale);
		static Mat3x2 create_rotation(float rotation);
		static Mat3x2 create_translation(const Vec2<float>& translation);
		static Mat3x2 create_transform(const Vec2<float>& position, float rotation, const Vec2<float>& scale, const Vec2<float>& origin);

		float* value_ptr();
        const float* value_ptr() const;

		float determinant() const;
		Mat3x2 inverse() const;

		Mat3x2 operator - (const Mat3x2& other) const;
		Mat3x2 operator + (const Mat3x2& other) const;
		Mat3x2 operator * (const Mat3x2& other) const;

		Mat3x2& operator -= (const Mat3x2& other);
		Mat3x2& operator += (const Mat3x2& other);
		Mat3x2& operator *= (const Mat3x2& other);

		float* operator [] (int idx);
		const float* operator [] (int idx) const;
	};
};

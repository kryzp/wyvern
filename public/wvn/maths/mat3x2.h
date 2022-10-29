#pragma once

namespace wvn
{
	template <typename T> struct Vec2;
	struct Mat4x4;

	/*
	 * Special matrix used for representing 2D transformations.
	 * This mathematically doesn't actually make sense because the
	 * naming is slightly dishonest.
	 *
	 * This is actually a 2x2 2D transformation matrix along with a
	 * 1x2 part representing the "offset" or "position", that gets
	 * transformed individually.
	 */
	struct Mat3x2
	{
		union
		{
			struct
			{
				float m11, m12;
				float m21, m22;
				float m31, m32;
			};

			float elements[3][2];
			float data[6];
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

		Vec2<float> position() const;

		float* value_ptr();
        const float* value_ptr() const;

		float determinant() const;
		Mat3x2 inverse() const;

		Mat3x2 operator - (const Mat3x2& other) const;
		Mat3x2 operator + (const Mat3x2& other) const;
		Mat3x2 operator * (float scalar       ) const;
		Mat3x2 operator * (const Mat3x2& other) const;

		Mat3x2& operator -= (const Mat3x2& other);
		Mat3x2& operator += (const Mat3x2& other);
		Mat3x2& operator *= (float scalar       );
		Mat3x2& operator *= (const Mat3x2& other);

		float* operator [] (int idx);
		const float* operator [] (int idx) const;
	};
};

#ifndef MAT2X3_H
#define MAT2X3_H

namespace wvn
{
	template <typename T> struct Vec2;

	/**
	 * Affine 2D transformation matrix.
	 */

	/*
	 * For reference:
	 *
	 * +---------+
	 * | 1 0 | x |
	 * | 0 1 | y |
	 * +---------+
	 */
	struct Mat2x3
	{
		union
		{
			struct
			{
				float m11, m12, m13;
				float m21, m22, m23;
			};

			float elements[2][3];
			float data[6];
		};

		Mat2x3();
		Mat2x3(const Mat2x3& other);
		Mat2x3(float diag);
		Mat2x3(
			float m11, float m12, float m13,
			float m21, float m22, float m23
		);

		static const Mat2x3& identity();

		static Vec2<float> offset(const Mat2x3& mat);

		static Mat2x3 create_skew(const Vec2<float>& amount);
		static Mat2x3 create_scale(const Vec2<float>& scale);
		static Mat2x3 create_scale(float scale);
		static Mat2x3 create_rotation(float angle);
		static Mat2x3 create_translation(const Vec2<float>& translation);
		static Mat2x3 create_transform(const Vec2<float>& position, float rotation, const Vec2<float>& scale, const Vec2<float>& origin);

		Vec2<float> position() const;

		float determinant() const;
		Mat2x3 inverse() const;

		Mat2x3 operator - (const Mat2x3& other) const;
		Mat2x3 operator + (const Mat2x3& other) const;
		Mat2x3 operator * (float scalar       ) const;
		Mat2x3 operator * (const Mat2x3& other) const;

		Mat2x3& operator -= (const Mat2x3& other);
		Mat2x3& operator += (const Mat2x3& other);
		Mat2x3& operator *= (float scalar       );
		Mat2x3& operator *= (const Mat2x3& other);

		float* operator [] (int idx);
		const float* operator [] (int idx) const;
	};
};

#endif // MAT2X3_H

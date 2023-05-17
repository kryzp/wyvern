#ifndef MAT2X3_H
#define MAT2X3_H

namespace wvn
{
	template <typename T>
	struct Vec2;
	using Vec2F = Vec2<float>;

	/**
	 * Affine 2D transformation matrix.
	 */

	/*
	 * For reference:
	 *
	 * +---------+
	 * | a b | x |
	 * | c d | y |
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

		static Vec2F offset(const Mat2x3& mat);

		static Mat2x3 create_skew(const Vec2F& amount);
		static Mat2x3 create_scale(const Vec2F& scale);
		static Mat2x3 create_scale(float scale);
		static Mat2x3 create_rotation(float angle);
		static Mat2x3 create_translation(const Vec2F& translation);
		static Mat2x3 create_transform(const Vec2F& position, float rotation, const Vec2F& scale, const Vec2F& origin);

		Vec2F position() const;

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

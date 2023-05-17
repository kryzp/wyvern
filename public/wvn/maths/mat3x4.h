#ifndef MAT3X4_H
#define MAT3X4_H

namespace wvn
{
	template <typename T>
	struct Vec3;
	using Vec3F = Vec3<float>;

	struct Quat;

	/**
	 * Affine 3D transformation matrix.
	 */

	/*
	 * For reference:
	 *
	 * +-----------+
	 * | a b c | x |
	 * | d e f | y |
	 * | g h i | z |
	 * +-----------+
	 */

	struct Mat3x4
	{
		union
		{
			struct
			{
//				float m11, m21, m31;
//				float m12, m22, m32;
//				float m13, m23, m33;
//				float m14, m24, m34;
				float m11, m12, m13, m14;
				float m21, m22, m23, m24;
				float m31, m32, m33, m34;
			};

			float elements[3][4];
			float data[12];
		};

		Mat3x4();
		Mat3x4(const Mat3x4& other);
		Mat3x4(float diag);
		Mat3x4(
			float m11, float m12, float m13, float m14,
			float m21, float m22, float m23, float m24,
			float m31, float m32, float m33, float m34
		);

		static const Mat3x4& identity();

		static Vec3F offset(const Mat3x4& mat);

		static Mat3x4 create_skew(const Vec3F& amount);
		static Mat3x4 create_scale(const Vec3F& scale);
		static Mat3x4 create_scale(float x, float y, float z);
		static Mat3x4 create_scale(float scale);
		static Mat3x4 create_rotation(const Quat& quat);
		static Mat3x4 create_rotation(const Vec3F& axis, float angle);
		static Mat3x4 create_translation(const Vec3F& translation);
		static Mat3x4 create_translation(float x, float y, float z);
		static Mat3x4 create_transform(const Vec3F& position, const Quat& rotation, const Vec3F& scale, const Vec3F& origin);

		Vec3F position() const;

		float determinant() const;
		Mat3x4 inverse() const;

		Mat3x4 operator - (const Mat3x4& other) const;
		Mat3x4 operator + (const Mat3x4& other) const;
		Mat3x4 operator * (float scalar       ) const;
		Mat3x4 operator * (const Mat3x4& other) const;
		Mat3x4 operator / (float scalar       ) const;

		Mat3x4& operator -= (const Mat3x4& other);
		Mat3x4& operator += (const Mat3x4& other);
		Mat3x4& operator *= (float scalar       );
		Mat3x4& operator *= (const Mat3x4& other);
		Mat3x4& operator /= (float scalar       );

		float* operator [] (int idx);
		const float* operator [] (int idx) const;
	};
}

#endif // MAT3X4_H

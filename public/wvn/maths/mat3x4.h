#ifndef MAT3X4_H
#define MAT3X4_H

namespace wvn
{
	template <typename T> struct Vec3;
	struct Quaternion;

	/**
	 * Affine 3D transformation matrix.
	 */

	/*
	 * For reference:
	 *
	 * +-----------+
	 * | 1 0 0 | x |
	 * | 0 1 0 | y |
	 * | 0 0 1 | z |
	 * +-----------+
	 */

	struct Mat3x4
	{
		union
		{
			struct
			{
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

		static Vec3<float> offset(const Mat3x4& mat);

		static Mat3x4 create_skew(const Vec3<float>& amount);
		static Mat3x4 create_scale(const Vec3<float>& scale);
		static Mat3x4 create_scale(float scale);
		static Mat3x4 create_rotation(const Quaternion& quat);
		static Mat3x4 create_rotation(const Vec3<float>& axis, float angle);
		static Mat3x4 create_translation(const Vec3<float>& translation);
		static Mat3x4 create_transform(const Vec3<float>& position, const Quaternion& rotation, const Vec3<float>& scale, const Vec3<float>& origin);

		Vec3<float> position() const;

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

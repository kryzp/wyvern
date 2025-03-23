#ifndef MAT3X3_H_
#define MAT3X3_H_

namespace wvn
{
	template <typename T>
	struct Vec3;
	using Vec3F = Vec3<float>;

	struct Quat;
	struct Mat4x4;

	/**
	 * Generic 3x3 matrix.
	 */
	struct Basis3D
	{
		union
		{
			struct
			{
				float m11, m21, m31;
				float m12, m22, m32;
				float m13, m23, m33;
			};

			float elements[3][3];
			float data[9];
		};

		Basis3D();
		Basis3D(const Basis3D& other);
		Basis3D(float diag);
		Basis3D(
			float m11, float m12, float m13,
			float m21, float m22, float m23,
			float m31, float m32, float m33
		);

		static const Basis3D& identity();

		static Vec3F transform(const Vec3F& vec, const Basis3D& mat);

		static Basis3D create_skew(const Vec3F& skew);
		static Basis3D create_scale(const Vec3F& scale);
		static Basis3D create_scale(float x, float y, float z);
		static Basis3D create_scale(float scale);
		static Basis3D create_rotation(const Quat& quat);
		static Basis3D create_rotation(const Vec3F& axis, float angle);

		float determinant() const;
		Basis3D inverse() const;
		Basis3D transpose() const;

		Mat4x4 as4x4() const;

		Basis3D operator - (const Basis3D& other) const;
		Basis3D operator + (const Basis3D& other) const;
		Basis3D operator / (float scalar        ) const;
		Basis3D operator * (float scalar        ) const;
		Basis3D operator * (const Basis3D& other) const;

		Basis3D& operator -= (const Basis3D& other);
		Basis3D& operator += (const Basis3D& other);
		Basis3D& operator /= (float scalar        );
		Basis3D& operator *= (float scalar        );
		Basis3D& operator *= (const Basis3D& other);
	};
}

#endif // MAT3X3_H_

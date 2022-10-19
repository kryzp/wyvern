#pragma once

namespace wvn
{
	/*
	 * Generic 3x3 matrix.
	 */
	class Mat3x3
	{
		union
		{
			struct
			{
				float m11, m12, m13;
				float m21, m22, m23;
				float m31, m32, m33;
			};

			float elements[3][3];
			float data[9];
		};

		Mat3x3();
		Mat3x3(const Mat3x3& other);
		Mat3x3(float initial);
		Mat3x3(
			float m11, float m12, float m13,
			float m21, float m22, float m23,
			float m31, float m32, float m33
		);

		static const Mat3x3& identity();

		float* value_ptr();
		const float* value_ptr() const;

		float determinant() const;
		Mat3x3 inverse() const;

		Mat3x3 operator - (const Mat3x3& other) const;
		Mat3x3 operator + (const Mat3x3& other) const;
		Mat3x3 operator * (float scalar       ) const;
		Mat3x3 operator * (const Mat3x3& other) const;

		Mat3x3& operator -= (const Mat3x3& other);
		Mat3x3& operator += (const Mat3x3& other);
		Mat3x3& operator *= (float scalar       );
		Mat3x3& operator *= (const Mat3x3& other);

		float* operator [] (int idx);
		const float* operator [] (int idx) const;
	};
}

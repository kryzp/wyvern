#ifndef QUATERNION_H
#define QUATERNION_H

namespace wvn
{
	template <typename T>
	struct Vec3;

	/**
	 * Used for representing rotations by black magic.
	 * Super interesting stuff!
	 */
	struct Quaternion
	{
		union
		{
			struct
			{
				float w;
				float x;
				float y;
				float z;
			};

			float data[4];
		};

		Quaternion();
		Quaternion(float x, float y, float z);
		Quaternion(float w, float x, float y, float z);
        Quaternion(const Quaternion& other);

		static const Quaternion& zero();
        static const Quaternion& one();
		static const Quaternion& identity();

		static Quaternion from_axis_angle(const Vec3<float>& axis, float angle);

		static Quaternion from_euler(float pitch, float yaw, float roll);
		static Vec3<float> to_euler(const Quaternion& quat);

		static float dot(const Quaternion& a, const Quaternion& b);

		Vec3<float> vector() const;

		Quaternion normalized() const;
		Quaternion inverse() const;

		float length_squared() const;
		float length() const;

		Quaternion rotate_on_axis(const Vec3<float>& axis, float angle) const;

		bool operator == (const Quaternion& other) const;
		bool operator != (const Quaternion& other) const;

		Quaternion operator + (const Quaternion& other) const;
		Quaternion operator - (const Quaternion& other) const;
		Quaternion operator * (const Quaternion& other) const;
        Quaternion operator * (float scalar) const;
        Quaternion operator / (float scalar) const;

		Quaternion operator - () const;

		Quaternion& operator += (const Quaternion& other);
		Quaternion& operator -= (const Quaternion& other);
		Quaternion& operator *= (const Quaternion& other);
	};
}

#endif // QUATERNION_H

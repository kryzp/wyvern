#ifndef QUATERNION_H
#define QUATERNION_H

namespace wvn
{
	template <typename T>
	struct Vec3;
	using Vec3F = Vec3<float>;

	/**
	 * Used for representing rotations by black magic.
	 */
	struct Quat
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

		Quat();
		Quat(float x, float y, float z);
		Quat(float w, float x, float y, float z);
        Quat(const Quat& other);

		static const Quat& zero();
        static const Quat& one();
		static const Quat& identity();

		static Quat from_axis_angle(const Vec3F& axis, float angle);

		static Quat from_euler(float pitch, float yaw, float roll);
		static Vec3F to_euler(const Quat& quat);

		static float dot(const Quat& a, const Quat& b);

		Vec3F vector() const;

		Quat normalized() const;
		Quat inverse() const;

		float length_squared() const;
		float length() const;

		Quat rotate_on_axis(const Vec3F& axis, float angle) const;

		bool operator == (const Quat& other) const;
		bool operator != (const Quat& other) const;

		Quat operator + (const Quat& other) const;
		Quat operator - (const Quat& other) const;
		Quat operator * (const Quat& other) const;
        Quat operator * (float scalar) const;
        Quat operator / (float scalar) const;

		Quat operator - () const;

		Quat& operator += (const Quat& other);
		Quat& operator -= (const Quat& other);
		Quat& operator *= (const Quat& other);
	};
}

#endif // QUATERNION_H

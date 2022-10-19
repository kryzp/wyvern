#pragma once

namespace wvn
{
	template <typename T> struct Vec3;

	/*
	 * Used for representing rotations via a 4D hypersphere.
	 * Yeah.
	 *
	 * Euler angles are just cringe /shrug
	 */
	struct Quaternion
	{
		union
		{
			struct
			{
				float s;
				float i;
				float j;
				float k;
			};

			float data[4];
		};

		Quaternion();
		Quaternion(float x);
		Quaternion(float s, float i, float j, float k);

		static const Quaternion& zero();
		static const Quaternion& one();

		static Quaternion from_euler(float pitch, float yaw, float roll);
		static Vec3<float> to_euler(const Quaternion& quat);

		Quaternion rotate_on_axis(float amount, const Vec3<float>& axis);

		float* value_ptr();
		const float* value_ptr() const;

		bool operator == (const Quaternion& other) const;
		bool operator != (const Quaternion& other) const;
	};
}

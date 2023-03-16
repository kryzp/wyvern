#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <wvn/maths/vec2.h>
#include <wvn/maths/vec3.h>
#include <wvn/maths/mat3x3.h>
#include <wvn/maths/quaternion.h>

#include <wvn/container/bitset.h>
#include <wvn/container/vector.h>

namespace wvn::phys
{
	enum CollisionDetection
	{
		COLLISION_DET_DISCRETE,
		COLLISION_DET_CONTINUOUS
	};

	enum SimulationType
	{
		SIM_TYPE_KINEMATIC,
		SIM_TYPE_DYNAMIC
	};

	/**
	 * RigidBody that holds the physics data required to
	 * actually calculate the physics such as mass, drag, position
	 * rotation, the stress tensor.
	 */
	class RigidBody
	{
		enum Flags
		{
			FLAG_DETECT_COLLISIONS,
			FLAG_IS_KINEMATIC,
			FLAG_USE_GRAVITY,
			FLAG_FREEZE_ROTATION,
			FLAG_IS_SLEEPING,
			FLAG_USE_MAX_SPEED
		};

	public:
		RigidBody();
		~RigidBody();

		void add_force(const Vec3F& force);
		void add_force_at_position(const Vec3F& force, const Vec3F& position);
		void add_force_relative(const Vec3F& force);

		void add_torque(const Vec3F& torque);
		void add_torque_relative(const Vec3F& torque);

		void set_position(const Point3& position);
		void move_position(const Vec3F& amount);

		void set_rotation(const Quaternion& rotation);
		void add_rotation(const Quaternion& rotation);

		bool is_sleeping();
		void sleep();
		void wake_up();

		Point3 closest_point_on_bounds(const Point3& point) const;

		void set_centre_of_mass(const Point3& position);
		void reset_centre_of_mass();

		void apply_forces();
		void dampen_forces();

	private:
		Bitset<16> m_flags;
		CollisionDetection m_collision_detection_mode;

		Quaternion m_rotation;
		float m_angular_drag;
		Vec3F m_angular_velocity;
		float m_max_angular_velocity;

		Point3 m_position;
		float m_drag;
		Vec3F m_velocity;
		float m_max_speed;
		Point3 m_centre_of_mass;

		float m_mass;
		Mat3x3 m_tensor;
	};
}

#endif // RIGIDBODY_H

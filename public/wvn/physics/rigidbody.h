#ifndef RIGID_BODY_H_
#define RIGID_BODY_H_

#include <wvn/maths/quat.h>
#include <wvn/maths/vec2.h>
#include <wvn/maths/vec3.h>
#include <wvn/maths/basis_3d.h>
#include <wvn/maths/affine_3d.h>

#include <wvn/container/bitset.h>
#include <wvn/container/vector.h>

namespace wvn::phys
{
	class PhysicsMgr;

	enum CollisionDetection
	{
		COLLISION_DET_NONE = -1,

		COLLISION_DET_DISCRETE,
		COLLISION_DET_CONTINUOUS,

		COLLISION_DET_MAX_ENUM
	};

	enum SimulationType
	{
		SIM_TYPE_NONE = -1,

		SIM_TYPE_KINEMATIC,
		SIM_TYPE_DYNAMIC,

		SIM_TYPE_MAX_ENUM
	};

	enum ForceApplyType
	{
		FORCE_APPLY_FORCE,
		FORCE_APPLY_IMPULSE,
		FORCE_APPLY_ACCEL
	};

	/**
	 * RigidBody that holds the physics data required to
	 * actually calculate the physics such as mass, drag, position
	 * rotation, the stress tensor.
	 */
	class RigidBody
	{
		friend class PhysicsMgr;

		enum Flags
		{
			FLAG_NONE = -1,

			FLAG_DETECT_COLLISIONS,
			FLAG_IS_KINEMATIC,
			FLAG_USE_GRAVITY,
			FLAG_FREEZE_ROTATION,
			FLAG_IS_SLEEPING,
			FLAG_USE_MAX_SPEED,

			FLAG_MAX_ENUM
		};

	public:
		RigidBody();
		~RigidBody();

		bool is_sleeping();
		void sleep();
		void wake_up();

		Affine3D get_matrix();

		// ==== LINEAR MOTION ==== //
		Vec3F get_position() const;
		void set_position(const Vec3F& position);
		void move_position(const Vec3F& amount);

		void add_force(const Vec3F& force, ForceApplyType type = FORCE_APPLY_FORCE);
		void add_force_at_point(const Vec3F& force, const Vec3F& point, ForceApplyType type = FORCE_APPLY_FORCE); // this assumes the point is relative to the center of mass
		void add_force_at_global_point(const Vec3F& force, const Vec3F& point, ForceApplyType type = FORCE_APPLY_FORCE);

		Vec3F get_centre_of_mass() const;
		void set_centre_of_mass(const Vec3F& position);
		void reset_centre_of_mass();

		void set_mass(float mass);
		float get_mass() const;

		void toggle_max_speed(bool enabled);
		void set_max_speed(float max_speed);
		void set_drag(float drag);

		Vec3F velocity_at_point(const Vec3F& point); // this assumes the point is relative to the center of mass
		Vec3F velocity_at_global_point(const Vec3F& point);

		Vec3F get_forces() const;

		Vec3F get_velocity() const;
		Vec3F get_acceleration() const;

		// ==== ANGULAR MOTION ==== /
		void set_inertia(float xx, float yy, float zz);

		Quat get_orientation() const;
		void set_orientation(const Quat& orientation);

		Vec3F get_angular_velocity() const;

		void toggle_max_angular_speed(bool enabled);
		void set_max_angular_speed(float max_angular_speed);
		void set_angular_drag(float angular_drag);

		void add_torque(const Vec3F& torque);

	private:
		void on_modified();

		// general settings
		Bitset<16> m_flags;
		CollisionDetection m_collision_detection_mode;
		Vec3F m_centre_of_mass;
		Affine3D m_matrix;
		bool m_dirty_matrix;

		// linear motion
		Vec3F m_position;
		Vec3F m_velocity;
		Vec3F m_acceleration;
		Vec3F m_momentum_change;
		float m_mass;
		float m_max_speed;
		bool m_speed_throttled;
		float m_drag;

		// angular motion
		Quat m_orientation;
		Vec3F m_angular_velocity;
		Vec3F m_angular_acceleration;
		Quat m_spin; // for lack of a better term - quaternion angular velocity
		Quat m_rotor; // for lack of a better term - quaternion angular acceleration
		Vec3F m_angular_momentum_change;
		Vec3F m_inertia;
		float m_max_angular_speed;
		bool m_angular_speed_throttled;
		float m_angular_drag;
	};
}

#endif // RIGID_BODY_H_

#include <wvn/physics/rigidbody.h>

using namespace wvn;
using namespace wvn::phys;

RigidBody::RigidBody()

	// general settings
//	: m_flags()
//	, m_collision_detection_mode(COLLISION_DET_CONTINUOUS)
//	, m_centre_of_mass(Vec3F::zero())
//	, m_dirty_matrix(true)
//
//	// linear motion
//	, m_position(Vec3F::zero())
//	, m_velocity(Vec3F::zero())
//	, m_mass(0.0f)
//	, m_max_speed(0.0f)
//	, m_speed_throttled(false)
//	, m_drag(0.0f)
//
//	// angular motion
//	, m_orientation(Quat::zero())
//	, m_angular_momentum(Vec3F::zero())
//	, m_spin(Quat::zero())
//	, m_angular_velocity(Vec3F::zero())
//	, m_inertia(Vec3F::zero())
//	, m_max_angular_speed(0.0f)
//	, m_angular_speed_throttled(false)
//	, m_angular_drag(0.0f)
{
}

RigidBody::~RigidBody()
{
}

bool RigidBody::is_sleeping()
{
	return m_flags.is_on(FLAG_IS_SLEEPING);
}

void RigidBody::sleep()
{
	m_flags.enable(FLAG_IS_SLEEPING);
}

void RigidBody::wake_up()
{
	m_flags.disable(FLAG_IS_SLEEPING);
}

Affine3D RigidBody::get_matrix()
{
	if (m_dirty_matrix)
	{
		m_matrix = Affine3D::create_transform(
			m_position,
			m_orientation,
			Vec3F(1.5f),
			Vec3F::zero()
		);

		m_dirty_matrix = false;
	}

	return m_matrix;
}

void RigidBody::on_modified()
{
	m_dirty_matrix = true;

//	m_velocity = m_momentum / m_mass;

//	if (m_speed_throttled && m_velocity.length_squared() > m_max_speed*m_max_speed) {
//		m_velocity *= m_max_speed / m_velocity.length();
//	}

//	m_angular_velocity = m_angular_momentum / m_inertia;

//	if (m_angular_speed_throttled && m_angular_velocity.length_squared() > m_max_angular_speed*m_max_angular_speed) {
//		m_angular_velocity *= m_max_angular_speed / m_angular_velocity.length();
//	}

	m_spin  = 0.5f * Quat(m_angular_velocity)     * m_orientation;
	m_rotor = 0.5f * Quat(m_angular_acceleration) * m_orientation;

	m_orientation = m_orientation.normalized();
}

Vec3F RigidBody::get_position() const
{
	return m_position;
}

void RigidBody::set_position(const Vec3F& position)
{
	m_position = position;
	on_modified();
}

void RigidBody::move_position(const Vec3F& amount)
{
	m_position += amount;
	on_modified();
}

void RigidBody::add_force(const Vec3F& force, ForceApplyType type)
{
	if (type == FORCE_APPLY_FORCE) {
		m_momentum_change += force * time::delta;
	} else if (type == FORCE_APPLY_IMPULSE) {
		m_momentum_change += force;
	} else if (type == FORCE_APPLY_ACCEL) {
		m_momentum_change += force * m_mass * time::delta;
	}

	on_modified();
}

void RigidBody::add_force_at_point(const Vec3F& force, const Vec3F& point, ForceApplyType type)
{
	if (type == FORCE_APPLY_FORCE) {
		m_momentum_change += force * time::delta;
		m_angular_momentum_change += Vec3F::cross(point, force) * time::delta;
	} else if (type == FORCE_APPLY_IMPULSE) {
		m_momentum_change += force;
		m_angular_momentum_change += Vec3F::cross(point, force);
	} else if (type == FORCE_APPLY_ACCEL) {
		m_momentum_change += force * m_mass * time::delta;
		m_angular_momentum_change += Vec3F::cross(point, force * m_mass) * time::delta;
	}

	on_modified();
}

void RigidBody::add_force_at_global_point(const Vec3F& force, const Vec3F& point, ForceApplyType type)
{
	add_force_at_point(force, point - m_centre_of_mass, type);
}

Vec3F RigidBody::get_centre_of_mass() const
{
	return m_centre_of_mass;
}

void RigidBody::set_centre_of_mass(const Vec3F& position)
{
	m_centre_of_mass = position;
}

void RigidBody::reset_centre_of_mass()
{
	m_centre_of_mass = Vec3F::zero();
}

void RigidBody::set_mass(float mass)
{
	m_mass = mass;
}

void RigidBody::set_inertia(float xx, float yy, float zz)
{
	m_inertia = { xx, yy, zz };
}

float RigidBody::get_mass() const
{
	return m_mass;
}

void RigidBody::toggle_max_speed(bool enabled)
{
	m_speed_throttled = enabled;
}

void RigidBody::set_max_speed(float max_speed)
{
	m_max_speed = max_speed;
}

void RigidBody::set_drag(float drag)
{
	m_drag = drag;
}

Vec3F RigidBody::velocity_at_point(const Vec3F& point)
{
	return m_velocity + Vec3F::cross(m_angular_velocity, point);
}

Vec3F RigidBody::velocity_at_global_point(const Vec3F& point)
{
	return velocity_at_point(point - m_centre_of_mass);
}

Vec3F RigidBody::get_forces() const
{
	return m_momentum_change;
}

Vec3F RigidBody::get_velocity() const
{
	return m_velocity;
}

Vec3F RigidBody::get_acceleration() const
{
	return m_acceleration;
}

Quat RigidBody::get_orientation() const
{
	return m_orientation;
}

void RigidBody::set_orientation(const Quat& orientation)
{
	m_orientation = orientation;
	on_modified();
}

Vec3F RigidBody::get_angular_velocity() const
{
	return m_angular_velocity;
}

void RigidBody::toggle_max_angular_speed(bool enabled)
{
	m_angular_speed_throttled = enabled;
}

void RigidBody::set_max_angular_speed(float max_angular_speed)
{
	m_max_angular_speed = max_angular_speed;
	on_modified();
}

void RigidBody::set_angular_drag(float angular_drag)
{
	m_angular_drag = angular_drag;
	on_modified();
}

void RigidBody::add_torque(const Vec3F& torque)
{
	m_angular_momentum_change += torque;
	on_modified();
}

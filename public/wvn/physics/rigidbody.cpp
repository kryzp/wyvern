#include <wvn/physics/rigidbody.h>
#include <wvn/maths/calc.h>

using namespace wvn;
using namespace wvn::phys;

RigidBody::RigidBody()
{
}

RigidBody::~RigidBody()
{
}

void RigidBody::add_force(const Vec3F& force)
{
	m_velocity += force / m_mass;
}

void RigidBody::add_force_at_position(const Vec3F& force, const Vec3F& position)
{
}

void RigidBody::add_force_relative(const Vec3F& force)
{
}

void RigidBody::add_torque(const Vec3F& torque)
{
	m_angular_velocity += torque;
}

void RigidBody::add_torque_relative(const Vec3F& torque)
{
}

void RigidBody::set_position(const Point3& position)
{
	m_position = position;
}

void RigidBody::move_position(const Vec3F& amount)
{
	m_position += amount;
}

void RigidBody::set_rotation(const Quaternion& rotation)
{
	m_rotation = rotation;
}

void RigidBody::add_rotation(const Quaternion& rotation)
{
}

bool RigidBody::is_sleeping()
{
	return m_flags.on(FLAG_IS_SLEEPING);
}

void RigidBody::sleep()
{
	m_flags.enable(FLAG_IS_SLEEPING);
}

void RigidBody::wake_up()
{
	m_flags.disable(FLAG_IS_SLEEPING);
}

Point3 RigidBody::closest_point_on_bounds(const Point3& point) const
{
	return Point3::zero();
}

void RigidBody::set_centre_of_mass(const wvn::Point3& position)
{
	m_centre_of_mass = position;
}

void RigidBody::reset_centre_of_mass()
{
	m_centre_of_mass = Point3::zero();
}

void RigidBody::apply_forces()
{
	m_position += m_velocity;
	//m_rotation += Quaternion::from_euler(m_angular_velocity.x, m_angular_velocity.y, m_angular_velocity.z);

	dampen_forces();
}

void RigidBody::dampen_forces()
{
	m_velocity *= 1.0f / m_drag;
	m_angular_velocity *= 1.0f / m_angular_drag;
}

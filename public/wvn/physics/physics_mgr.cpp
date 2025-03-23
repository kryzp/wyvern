#include <wvn/physics/physics_mgr.h>
#include <wvn/devenv/log_mgr.h>

#include <wvn/physics/agents/agent_sphere_sphere.h>

using namespace wvn;
using namespace wvn::phys;

wvn_IMPL_SINGLETON(PhysicsMgr);

PhysicsMgr::PhysicsMgr()
	: m_rigid_bodies()
	, m_agent_registry()
{
	init_agent_registry();

	dev::LogMgr::get_singleton()->print("[PHYSICS] Initialized!");
}

void PhysicsMgr::init_agent_registry()
{
	m_agent_registry.add<AgentSphereSphere>(
		COLLISION_SHAPE_TYPE_SPHERE, COLLISION_SHAPE_TYPE_SPHERE
	);
}

PhysicsMgr::~PhysicsMgr()
{
	dev::LogMgr::get_singleton()->print("[PHYSICS] Destroyed!");
}

RigidBody* PhysicsMgr::create_rigidbody()
{
	RigidBody* rb = new RigidBody();
	m_rigid_bodies.push_back(rb);
	return rb;
}

void PhysicsMgr::simulate()
{
	const float dt = time::delta;

	for (int i = 0; i < m_rigid_bodies.size(); i++)
	{
		auto rb = m_rigid_bodies[i];
		rb->on_modified(); // ensure data is up-to-date

		// linear
		Vec3F new_pos = rb->m_position + rb->m_velocity*dt + 0.5f*rb->m_acceleration*dt*dt;
		Vec3F new_vel_half = rb->m_velocity + 0.5f*rb->m_acceleration*dt;
		Vec3F new_acc = rb->m_momentum_change / rb->m_mass;
		Vec3F new_vel = new_vel_half + 0.5f*new_acc*dt;

		rb->m_position = new_pos;
		rb->m_velocity = new_vel;
		rb->m_acceleration = new_acc;

		// angular
		Quat new_ori = rb->m_orientation + rb->m_spin*dt + 0.5f*rb->m_rotor*dt*dt;
		Quat new_omega_half = rb->m_spin + 0.5f*rb->m_rotor*dt;
		Quat new_alpha = rb->m_angular_momentum_change / rb->m_inertia;
		Quat new_omega = new_omega_half + 0.5f*new_alpha*dt;

		if (!CalcF::within_epsilon(0.0f, new_ori.length()))
			rb->m_orientation = new_ori.normalized();

		if (!CalcF::within_epsilon(0.0f, new_omega.length()))
			rb->m_angular_velocity = 2.0f * (new_omega.normalized() * new_ori.inverse()).vector();

		if (!CalcF::within_epsilon(0.0f, new_alpha.length()))
			rb->m_angular_acceleration = 2.0f * (new_alpha.normalized() * new_ori.inverse()).vector();
	}

	// update key-framed rigid bodies

	// update phantoms

	// update forces & adjust constraints

	// step the simulation

	// update dynamic rigid bodies

	// query phantoms

	// perform collision cast queries
}

void PhysicsMgr::finalize()
{
	for (int i = 0; i < m_rigid_bodies.size(); i++) {
		m_rigid_bodies[i]->m_momentum_change = Vec3F::zero();
		m_rigid_bodies[i]->m_angular_momentum_change = Vec3F::zero();
	}
}

#if 0

{
        Vec3d new_pos = pos + vel*dt + acc*(dt*dt*0.5);
        Vec3d new_acc = apply_forces(); // only needed if acceleration is not constant
        Vec3d new_vel = vel + (acc+new_acc)*(dt*0.5);
        pos = new_pos;
        vel = new_vel;
        acc = new_acc;

void ApplyLinearCollisionImpulse( StaticContact & contact, float e )
{
	float mass = contact.rigidBody->mass;
	float d = dot( contact.rigidBody->linearMomentum, contact.normal );
	float j = max( - ( 1 + e ) * d, 0 );
	contact.rigidBody->linearMomentum += j * contact.normal;
}

#endif // 0

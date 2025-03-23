#ifndef PHYSICS_MGR_H_
#define PHYSICS_MGR_H_

#include <wvn/singleton.h>
#include <wvn/container/vector.h>

#include <wvn/physics/rigidbody.h>
#include <wvn/physics/agent_registry.h>

namespace wvn::phys
{
	/**
	 * Manages the phyiscs of the game world
	 */
	class PhysicsMgr : public Singleton<PhysicsMgr>
	{
		wvn_DEF_SINGLETON(PhysicsMgr)

	public:
		PhysicsMgr();
		~PhysicsMgr();

		void simulate();
		void finalize();

		RigidBody* create_rigidbody();

	private:
		void init_agent_registry();

		Vector<RigidBody*> m_rigid_bodies;
		AgentRegistry m_agent_registry;
	};
}

#endif // PHYSICS_MGR_H_

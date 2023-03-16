#ifndef PHYSICS_MGR_H
#define PHYSICS_MGR_H

#include <wvn/util/singleton.h>

namespace wvn::phys
{
	/**
	 * Manages the phyiscs of the game world
	 */
	class PhysicsMgr : public Singleton<PhysicsMgr>
	{
		WVN_DEF_SINGLETON(PhysicsMgr)

	public:
		PhysicsMgr();
		~PhysicsMgr();

		void simulate();
	};
}

#endif // PHYSICS_MGR_H

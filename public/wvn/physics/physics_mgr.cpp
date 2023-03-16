#include <wvn/physics/physics_mgr.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::phys;

WVN_IMPL_SINGLETON(PhysicsMgr);

PhysicsMgr::PhysicsMgr()
{
	dev::LogMgr::get_singleton()->print("[PHYSICS] Initialized!");
}

PhysicsMgr::~PhysicsMgr()
{
	dev::LogMgr::get_singleton()->print("[PHYSICS] Destroyed!");
}

void PhysicsMgr::simulate()
{
	// update key-framed rigid bodies

	// update phantoms

	// update forces / impulses & adjust constraints

	// step the simulation

	// update dynamic rigid bodies

	// query phantoms

	// perform collision cast queries
}

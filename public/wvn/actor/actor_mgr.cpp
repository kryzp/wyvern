#include <wvn/actor/actor_mgr.h>

using namespace wvn;

WVN_IMPL_SINGLETON(ActorMgr);

ActorMgr::ActorMgr()
{
}

ActorMgr::~ActorMgr()
{
	for (auto& act : m_actors)
		delete act;

	m_actors.clear();
}

void ActorMgr::tick()
{
	for (auto& act : m_actors)
		act->tick();
}

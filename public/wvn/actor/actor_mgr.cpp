#include <wvn/actor/actor_mgr.h>
#include <wvn/actor/actor.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::act;

WVN_IMPL_SINGLETON(ActorMgr);

ActorMgr::ActorMgr()
	: m_actors(ACTOR_BUCKETS)
	, m_actors_initializing()
	, m_actors_destroying()
	, m_free_ids()
	, m_unique_id(Actor::NULL_ID + 1)
{
	dev::LogMgr::get_singleton()->print("[ACTOR] Initialized!");
}

ActorMgr::~ActorMgr()
{
	for (auto& [id, act] : m_actors) {
		act->destroy();
	}

	for (auto& [id, act] : m_actors) {
		delete act;
	}

	m_actors.clear();
	m_actors_initializing.clear();
	m_actors_destroying.clear();

	m_free_ids.clear();
	m_unique_id = Actor::NULL_ID;

	dev::LogMgr::get_singleton()->print("[ACTOR] Destroyed!");
}

void ActorMgr::tick_pre_animation()
{
	resolve_initializing();

	for (auto& [id, act] : m_actors) {
		act->tick();
	}

	resolve_removing();
}

void ActorMgr::tick_post_animation()
{
}

void ActorMgr::resolve_initializing()
{
	for (auto& act : m_actors_initializing) {
		act->init();
	}

	m_actors_initializing.clear();
}

void ActorMgr::resolve_removing()
{
	for (auto& act : m_actors_destroying) {
		act->destroy();
	}

	for (auto& act : m_actors_destroying)
	{
		delete act.get();
		m_actors.erase(act.id());
		m_free_ids.push_back(act.id());
	}

	m_actors_destroying.clear();
}

void ActorMgr::destroy(const ActorHandle& act)
{
	m_actors_destroying.push_back(act);
}

bool ActorMgr::is_valid(const ActorHandle& act)
{
	if (act.id() == Actor::NULL_ID) {
		return false;
	}

	return m_actors.contains(act.id());
}

Actor* ActorMgr::fetch(const ActorHandle& act)
{
	return m_actors[act.id()];
}

void ActorMgr::foreach(const Function<void(ActorHandle&)>& fn)
{
	for (auto& [id, act] : m_actors)
	{
		ActorHandle handle = ActorHandle(act);
		fn(handle);
	}
}

void ActorMgr::foreach(u64 mask, const Function<void(ActorHandle&)>& fn)
{
	for (auto& [id, act] : m_actors)
	{
		if (act->has_flag(mask))
		{
			ActorHandle handle = ActorHandle(act);
			fn(handle);
		}
	}
}

void ActorMgr::foreach_stoppable(const Function<bool(ActorHandle&)>& fn)
{
	for (auto& [id, act] : m_actors)
	{
		ActorHandle handle = ActorHandle(act);

		if (fn(handle)) {
			return;
		}
	}
}

void ActorMgr::foreach_stoppable(u64 mask, const Function<bool(ActorHandle&)>& fn)
{
	for (auto& [id, act] : m_actors)
	{
		if (act->has_flag(mask))
		{
			ActorHandle handle = ActorHandle(act);

			if (fn(handle)) {
				return;
			}
		}
	}
}

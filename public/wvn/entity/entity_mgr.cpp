#include <wvn/entity/entity_mgr.h>
#include <wvn/entity/entity.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::ent;

wvn_IMPL_SINGLETON(EntityMgr);

EntityMgr::EntityMgr()
	: m_entities(ENTITY_BUCKETS)
	, m_entities_initializing()
	, m_entities_destroying()
	, m_free_ids()
	, m_unique_id(Entity::NULL_ID + 1)
{
	dev::LogMgr::get_singleton()->print("[ACTOR] Initialized!");
}

EntityMgr::~EntityMgr()
{
	for (auto& [id, ent] : m_entities) {
		ent->destroy();
	}

	for (auto& [id, ent] : m_entities) {
		delete ent;
	}

	m_entities.clear();
	m_entities_initializing.clear();
	m_entities_destroying.clear();

	m_free_ids.clear();
	m_unique_id = Entity::NULL_ID;

	dev::LogMgr::get_singleton()->print("[ACTOR] Destroyed!");
}

void EntityMgr::tick_pre_animation()
{
	resolve_initializing();

	for (auto& [id, ent] : m_entities) {
		ent->tick();
	}

	resolve_removing();
}

void EntityMgr::tick_post_animation()
{
}

void EntityMgr::tick_post_physics_update()
{
}

void EntityMgr::resolve_initializing()
{
	for (auto& ent : m_entities_initializing) {
		ent->init();
	}

	m_entities_initializing.clear();
}

void EntityMgr::resolve_removing()
{
	for (auto& ent : m_entities_destroying) {
		ent->destroy();
	}

	for (auto& ent : m_entities_destroying)
	{
		delete ent.get();
		m_entities.erase(ent.id());
		m_free_ids.push_back(ent.id());
	}

	m_entities_destroying.clear();
}

void EntityMgr::destroy(const EntityHandle& ent)
{
	m_entities_destroying.push_back(ent);
}

bool EntityMgr::is_valid(const EntityHandle& ent)
{
	if (ent.id() == Entity::NULL_ID) {
		return false;
	}

	return m_entities.contains(ent.id());
}

Entity* EntityMgr::fetch(const EntityHandle& ent)
{
	return m_entities[ent.id()];
}

void EntityMgr::foreach(const Function<void(EntityHandle&)>& fn)
{
	for (auto& [id, ent] : m_entities)
	{
		EntityHandle handle = EntityHandle(ent);
		fn(handle);
	}
}

void EntityMgr::foreach(u64 mask, const Function<void(EntityHandle&)>& fn)
{
	for (auto& [id, ent] : m_entities)
	{
		if (ent->has_flag(mask))
		{
			EntityHandle handle = EntityHandle(ent);
			fn(handle);
		}
	}
}

void EntityMgr::foreach_stoppable(const Function<bool(EntityHandle&)>& fn)
{
	for (auto& [id, ent] : m_entities)
	{
		EntityHandle handle = EntityHandle(ent);

		if (fn(handle)) {
			return;
		}
	}
}

void EntityMgr::foreach_stoppable(u64 mask, const Function<bool(EntityHandle&)>& fn)
{
	for (auto& [id, ent] : m_entities)
	{
		if (ent->has_flag(mask))
		{
			EntityHandle handle = EntityHandle(ent);

			if (fn(handle)) {
				return;
			}
		}
	}
}

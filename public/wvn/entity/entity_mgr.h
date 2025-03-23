#ifndef ENTITY_MGR_H_
#define ENTITY_MGR_H_

#include <wvn/singleton.h>

#include <wvn/container/vector.h>
#include <wvn/container/hash_map.h>
#include <wvn/container/deque.h>
#include <wvn/container/function.h>

#include <wvn/entity/entity.h>
#include <wvn/entity/entity_handle.h>
#include <wvn/entity/entity_common.h>

namespace wvn::ent
{
	// todo: entity iterator?

	/**
	 * Manages entities in the game world.
	 */
	class EntityMgr : public Singleton<EntityMgr>
	{
		wvn_DEF_SINGLETON(EntityMgr);

		friend class EntityHandle;

	public:
		constexpr static int ENTITY_BUCKETS = 64;

		EntityMgr();
		~EntityMgr();

		void tick_pre_animation();
		void tick_post_animation();
		void tick_post_physics_update();

		// temporary //
		template <typename T, typename... Args>
		EntityHandle create(Args&&... args);
		// temporary //

		void destroy(const EntityHandle& ent);

		bool is_valid(const EntityHandle& ent);
		Entity* fetch(const EntityHandle& ent);

		void foreach(const Function<void(EntityHandle&)>& fn);
		void foreach(u64 mask, const Function<void(EntityHandle&)>& fn);
		void foreach_stoppable(const Function<bool(EntityHandle&)>& fn);
		void foreach_stoppable(u64 mask, const Function<bool(EntityHandle&)>& fn);

	private:
		void resolve_initializing();
		void resolve_removing();

		HashMap<EntityID, Entity*> m_entities;
		Vector<EntityHandle> m_entities_initializing;
		Vector<EntityHandle> m_entities_destroying;

		Deque<EntityID> m_free_ids;
		EntityID m_unique_id;
	};

	template <typename T, typename... Args>
	EntityHandle EntityMgr::create(Args&&... args)
	{
		EntityID ent_id = Entity::NULL_ID;

		if (!m_free_ids.empty())
		{
			ent_id = m_free_ids.pop_front();
		}
		else
		{
			ent_id = m_unique_id;
			m_unique_id++;
		}

		T* entity = new T(std::forward<Args>(args)...);
		entity->m_flags = 0;
		entity->m_id = ent_id;

		m_entities_initializing.push_back(entity);
		m_entities.insert(Pair(ent_id, static_cast<Entity*>(entity)));

		return EntityHandle(entity);
	}
}

#endif // ENTITY_MGR_H_

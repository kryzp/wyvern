#pragma once

#include <wvn/util/singleton.h>

#include <wvn/container/vector.h>
#include <wvn/container/hash_map.h>
#include <wvn/container/queue.h>
#include <wvn/container/function.h>

#include <wvn/actor/actor.h>

namespace wvn::act
{
	/*
	 * Manages actors in the game world.
	 */
	class ActorMgr : public Singleton<ActorMgr>
	{
		WVN_DEF_SINGLETON(ActorMgr);

		friend class ActorHandle;

	public:
		ActorMgr();
		~ActorMgr();

		void tick();

		// temporary //
		template <typename T, typename... Args>
		T* create(Args&&... args);
		// temporary //

		void destroy(const ActorHandle& act);

		bool is_valid(const ActorHandle& act);
		Actor* fetch(const ActorHandle& act);

		void foreach(const Function<void(ActorHandle&)>& fn);
		void foreach(u64 mask, const Function<void(ActorHandle&)>& fn);
		void foreach_stoppable(const Function<bool(ActorHandle&)>& fn);
		void foreach_stoppable(u64 mask, const Function<bool(ActorHandle&)>& fn);

	private:
		void resolve_initializing();
		void resolve_removing();

		HashMap<ActorID, Actor*> m_actors;
		Vector<ActorHandle> m_actors_initializing;
		Vector<ActorHandle> m_actors_destroying;

		Queue<ActorID> m_free_ids;
		ActorID m_unique_id;
	};

	template <typename T, typename... Args>
	T* ActorMgr::create(Args&& ...args)
	{
		ActorID act_id = Actor::INVALID_ID;

		if (!m_free_ids.empty())
		{
			act_id = m_free_ids.pop();
		}
		else
		{
			act_id = m_unique_id;
			m_unique_id++;
		}

		T* actor = new T(std::forward<Args>(args)...);
		actor->m_flags = 0;
		actor->m_id = act_id;

		m_actors_initializing.push_back(actor);
		m_actors.insert(Pair(act_id, static_cast<Actor*>(actor)));

		return actor;
	}
}

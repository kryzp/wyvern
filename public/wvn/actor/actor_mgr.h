#pragma once

#include <wvn/util/singleton.h>
#include <wvn/container/vector.h>

#include <wvn/actor/actor.h>

namespace wvn
{
	class ActorMgr : public Singleton<ActorMgr>
	{
		WVN_DEF_SINGLETON(ActorMgr)

	public:
		ActorMgr();
		~ActorMgr();

		void tick();

		// temporary
		template <class T, typename... Args>
		T* add(Args&&... args)
		{
			T* act = new T(std::forward<Args>(args)...);
			m_actors.push_back(act);
			return act;
		}

	private:
		wvn::Vector<Actor*> m_actors;
	};
}

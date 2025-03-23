#ifndef EVENT_MGR_H_
#define EVENT_MGR_H_

#include <wvn/singleton.h>
#include <wvn/entity/event.h>
#include <wvn/container/deque.h>

namespace wvn::ent
{
	/**
	 * Manages the flow of events in the entity system, dispatching
	 * them to all entities effectively.
	 */
	class EventMgr : public Singleton<EventMgr>
	{
		wvn_DEF_SINGLETON(EventMgr);

	public:
		EventMgr();
		~EventMgr();

		void enqueue_event(const Event& e);
		void dispatch_events();

	private:
		Deque<Event*> m_events;
	};
}

#endif // EVENT_MGR_H_

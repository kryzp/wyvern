#ifndef EVENT_MGR_H
#define EVENT_MGR_H

#include <wvn/util/singleton.h>
#include <wvn/actor/event.h>
#include <wvn/container/deque.h>

namespace wvn::act
{
	/**
	 * Manages the flow of events in the actor system, dispatching
	 * them to all actors effectively.
	 */
	class EventMgr : public Singleton<EventMgr>
	{
		WVN_DEF_SINGLETON(EventMgr);

	public:
		EventMgr();
		~EventMgr();

		void enqueue_event(Event* event);
		void dispatch_events();

	private:
		Deque<Event*> m_events;
	};
}

#endif // EVENT_MGR_H

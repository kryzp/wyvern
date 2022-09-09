#pragma once

#include <wvn/util/singleton.h>
#include <wvn/actor/event.h>
#include <wvn/container/queue.h>

namespace wvn::act
{
	class EventMgr : public Singleton<EventMgr>
	{
		WVN_DEF_SINGLETON(EventMgr);

		constexpr static u64 MAX_EVENTS = 128;

	public:
		EventMgr();
		~EventMgr();

		void enqueue_event(Event* event);
		void dispatch_events();

	private:
		Queue<Event*> m_events;
	};
}

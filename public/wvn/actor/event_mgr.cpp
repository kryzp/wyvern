#include <wvn/actor/event_mgr.h>

using namespace wvn;

WVN_IMPL_SINGLETON(EventMgr);

EventMgr::EventMgr()
	: m_events(MAX_EVENTS)
{
}

EventMgr::~EventMgr()
{
}

void EventMgr::enqueue_event(wvn::Event* event)
{
	m_events.push(event);
}

void EventMgr::dispatch_events()
{
	while (!m_events.empty())
	{
		Event* event = m_events.pop();
		event->dispatch();
		delete event;
	}
}

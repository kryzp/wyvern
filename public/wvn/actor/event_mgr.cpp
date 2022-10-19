#include <wvn/actor/event_mgr.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::act;

WVN_IMPL_SINGLETON(EventMgr);

EventMgr::EventMgr()
	: m_events(MAX_EVENTS)
{
	dev::LogMgr::get_singleton().print("[EVENT] Initialized!");
}

EventMgr::~EventMgr()
{
	dev::LogMgr::get_singleton().print("[EVENT] Destroyed!");
}

void EventMgr::enqueue_event(Event* event)
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

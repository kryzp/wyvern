#include <wvn/entity/event_mgr.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::ent;

wvn_IMPL_SINGLETON(EventMgr);

EventMgr::EventMgr()
	: m_events()
{
	dev::LogMgr::get_singleton()->print("[EVENT] Initialized!");
}

EventMgr::~EventMgr()
{
	dev::LogMgr::get_singleton()->print("[EVENT] Destroyed!");
}

void EventMgr::enqueue_event(const Event& e)
{
	m_events.push_back(new Event(e.type, e.args, e.receiver));
}

void EventMgr::dispatch_events()
{
	while (!m_events.empty()) {
		Event* e = m_events.pop_front();
		e->dispatch();
		delete e;
	}
}

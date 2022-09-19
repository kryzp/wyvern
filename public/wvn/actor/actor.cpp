#include <wvn/actor/actor.h>
#include <wvn/actor/actor_mgr.h>
#include <wvn/actor/event.h>

using namespace wvn;
using namespace wvn::act;

Actor::Actor()
	: m_flags()
	, m_id(INVALID_ID)
{
}

bool Actor::on_event(Event& e)
{
	return false;
}

void Actor::add_flag(u64 flag)
{
	m_flags |= flag;
}

void Actor::remove_flag(u64 flag)
{
	m_flags &= ~flag;
}

void Actor::toggle_flag(u64 flag)
{
	m_flags ^= flag;
}

void Actor::set_flag(u64 flag, bool mode)
{
	if (mode) {
		m_flags |= flag;
	} else {
		m_flags &= ~flag;
	}
}

bool Actor::has_flag(u64 flag) const
{
	return (m_flags & flag) != 0;
}

bool Actor::only_has_flag(u64 flag) const
{
	return (m_flags & flag) == flag;
}

void Actor::clear_flags()
{
	m_flags = 0;
}

/***********************************/

ActorHandle::ActorHandle()
	: m_id(Actor::INVALID_ID)
{
}

ActorHandle::ActorHandle(const Actor* actor)
	: m_id(actor->m_id)
{
}

ActorHandle::ActorHandle(ActorID id)
	: m_id(id)
{
}

ActorHandle::ActorHandle(const ActorHandle& other)
	: m_id(other.m_id)
{
}

ActorHandle::ActorHandle(ActorHandle&& other) noexcept
	: m_id(std::move(other.m_id))
{
	other.m_id = Actor::INVALID_ID;
}

ActorHandle& ActorHandle::operator = (const ActorHandle& other)
{
	this->m_id = other.m_id;
	return *this;
}

ActorHandle& ActorHandle::operator = (ActorHandle&& other) noexcept
{
	this->m_id = std::move(other.m_id);
	other.m_id = 0;
	return *this;
}

bool ActorHandle::is_valid() const
{
	return ActorMgr::get_singleton().is_valid(*this);
}

ActorHandle::operator bool () const
{
	return ActorMgr::get_singleton().is_valid(*this);
}

Actor* ActorHandle::get()
{
	return ActorMgr::get_singleton().fetch(*this);
}

const Actor* ActorHandle::get() const
{
	return ActorMgr::get_singleton().fetch(*this);
}

Actor* ActorHandle::operator -> ()
{
	return ActorMgr::get_singleton().fetch(*this);
}

const Actor* ActorHandle::operator -> () const
{
	return ActorMgr::get_singleton().fetch(*this);
}

ActorID ActorHandle::id() const
{
	return m_id;
}

ActorHandle::operator ActorID () const
{
	return m_id;
}

bool ActorHandle::operator == (const ActorHandle& other) const
{
	return this->m_id == other.m_id;
}

bool ActorHandle::operator != (const ActorHandle& other) const
{
	return !(*this == other);
}

#include <wvn/actor/actor_handle.h>
#include <wvn/actor/actor_mgr.h>

using namespace wvn;
using namespace wvn::act;

ActorHandle::ActorHandle()
	: m_id(Actor::NULL_ID)
{
}

ActorHandle::ActorHandle(const Actor* actor)
	: m_id(actor->m_id)
{
}

ActorHandle::ActorHandle(u32 id)
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
	other.m_id = Actor::NULL_ID;
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
	return ActorMgr::get_singleton()->is_valid(*this);
}

ActorHandle::operator bool () const
{
	return ActorMgr::get_singleton()->is_valid(*this);
}

Actor* ActorHandle::get()
{
	return ActorMgr::get_singleton()->fetch(*this);
}

const Actor* ActorHandle::get() const
{
	return ActorMgr::get_singleton()->fetch(*this);
}

Actor* ActorHandle::operator -> ()
{
	return ActorMgr::get_singleton()->fetch(*this);
}

const Actor* ActorHandle::operator -> () const
{
	return ActorMgr::get_singleton()->fetch(*this);
}

u32 ActorHandle::id() const
{
	return m_id;
}

ActorHandle::operator u32 () const
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

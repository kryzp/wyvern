#include <wvn/actor/actor.h>
#include <wvn/actor/actor_mgr.h>
#include <wvn/actor/event.h>
#include <wvn/graphics/model.h>

using namespace wvn;
using namespace wvn::act;

Actor::Actor()
	: m_flags(0)
	, m_id(NULL_ID)
{
}

void Actor::tick()
{
}

Transform3D& Actor::get_transform()
{
	return p_transform;
}

gfx::Model* Actor::get_model()
{
	return p_model;
}

// flag stuff

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

u64 Actor::flags() const
{
	return m_flags;
}

u32 Actor::id() const
{
	return m_id;
}

// event stuff

bool Actor::on_event(Event& e)
{
	return false;
}

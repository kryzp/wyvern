#include <wvn/entity/entity.h>
#include <wvn/entity/entity_mgr.h>
#include <wvn/entity/event.h>
#include <wvn/graphics/mesh.h>

using namespace wvn;
using namespace wvn::ent;

Entity::Entity()
	: m_flags(0)
	, m_id(NULL_ID)
    , m_transform()
    , m_rigidbody(nullptr)
    , m_renderable_object(nullptr)
    , m_velocity()
    , m_angular_velocity()
{
}

void Entity::tick()
{
}

Transform3D& Entity::get_transform()
{
	return m_transform;
}

gfx::RenderableObjectHandle Entity::get_renderable_object()
{
	return m_renderable_object;
}

// flag stuff

void Entity::add_flag(u64 flag)
{
	m_flags |= flag;
}

void Entity::remove_flag(u64 flag)
{
	m_flags &= ~flag;
}

void Entity::toggle_flag(u64 flag)
{
	m_flags ^= flag;
}

void Entity::set_flag(u64 flag, bool mode)
{
	if (mode) {
		m_flags |= flag;
	} else {
		m_flags &= ~flag;
	}
}

bool Entity::has_flag(u64 flag) const
{
	return (m_flags & flag) != 0;
}

bool Entity::only_has_flag(u64 flag) const
{
	return (m_flags & flag) == flag;
}

void Entity::clear_flags()
{
	m_flags = 0;
}

u64 Entity::flags() const
{
	return m_flags;
}

EntityID Entity::id() const
{
	return m_id;
}

// event stuff

bool Entity::on_event(const Event& e)
{
	return false;
}

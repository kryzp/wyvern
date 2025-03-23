#include <wvn/entity/entity_handle.h>
#include <wvn/entity/entity_mgr.h>

using namespace wvn;
using namespace wvn::ent;

EntityHandle::EntityHandle()
	: m_id(Entity::NULL_ID)
{
}

EntityHandle::EntityHandle(const Entity* entity)
	: m_id(entity ? entity->m_id : 0)
{
}

EntityHandle::EntityHandle(EntityID id)
	: m_id(id)
{
}

EntityHandle::EntityHandle(const EntityHandle& other)
	: m_id(other.m_id)
{
}

EntityHandle::EntityHandle(EntityHandle&& other) noexcept
	: m_id(std::move(other.m_id))
{
	other.m_id = Entity::NULL_ID;
}

EntityHandle& EntityHandle::operator = (const EntityHandle& other)
{
	this->m_id = other.m_id;
	return *this;
}

EntityHandle& EntityHandle::operator = (EntityHandle&& other) noexcept
{
	this->m_id = std::move(other.m_id);
	other.m_id = 0;
	return *this;
}

bool EntityHandle::is_valid() const
{
	return EntityMgr::get_singleton()->is_valid(*this);
}

EntityHandle::operator bool () const
{
	return EntityMgr::get_singleton()->is_valid(*this);
}

Entity* EntityHandle::get()
{
	return EntityMgr::get_singleton()->fetch(*this);
}

const Entity* EntityHandle::get() const
{
	return EntityMgr::get_singleton()->fetch(*this);
}

Entity* EntityHandle::operator -> ()
{
	return EntityMgr::get_singleton()->fetch(*this);
}

const Entity* EntityHandle::operator -> () const
{
	return EntityMgr::get_singleton()->fetch(*this);
}

EntityID EntityHandle::id() const
{
	return m_id;
}

EntityHandle::operator EntityID () const
{
	return m_id;
}

bool EntityHandle::operator == (const EntityHandle& other) const
{
	return this->m_id == other.m_id;
}

bool EntityHandle::operator != (const EntityHandle& other) const
{
	return !(*this == other);
}

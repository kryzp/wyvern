#include <wvn/graphics/renderable_object.h>
#include <wvn/graphics/rendering_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

RenderableObjectHandle::RenderableObjectHandle()
	: m_id(RenderableObject::NULL_ID)
{
}

RenderableObjectHandle::RenderableObjectHandle(const RenderableObject* object)
	: m_id(object ? object->id : 0)
{
}

RenderableObjectHandle::RenderableObjectHandle(RenderableObjectID id)
	: m_id(id)
{
}

RenderableObjectHandle::RenderableObjectHandle(const RenderableObjectHandle& other)
	: m_id(other.m_id)
{
}

RenderableObjectHandle::RenderableObjectHandle(RenderableObjectHandle&& other) noexcept
	: m_id(std::move(other.m_id))
{
	other.m_id = RenderableObject::NULL_ID;
}

RenderableObjectHandle& RenderableObjectHandle::operator = (const RenderableObjectHandle& other)
{
	this->m_id = other.m_id;
	return *this;
}

RenderableObjectHandle& RenderableObjectHandle::operator = (RenderableObjectHandle&& other) noexcept
{
	this->m_id = std::move(other.m_id);
	other.m_id = 0;
	return *this;
}

bool RenderableObjectHandle::is_valid() const
{
	return RenderingMgr::get_singleton()->is_valid_object(*this);
}

RenderableObjectHandle::operator bool () const
{
	return RenderingMgr::get_singleton()->is_valid_object(*this);
}

RenderableObject* RenderableObjectHandle::get()
{
	return RenderingMgr::get_singleton()->fetch_object(*this);
}

const RenderableObject* RenderableObjectHandle::get() const
{
	return RenderingMgr::get_singleton()->fetch_object(*this);
}

RenderableObject* RenderableObjectHandle::operator -> ()
{
	return RenderingMgr::get_singleton()->fetch_object(*this);
}

const RenderableObject* RenderableObjectHandle::operator -> () const
{
	return RenderingMgr::get_singleton()->fetch_object(*this);
}

RenderableObjectID RenderableObjectHandle::id() const
{
	return m_id;
}

RenderableObjectHandle::operator RenderableObjectID () const
{
	return m_id;
}

bool RenderableObjectHandle::operator == (const RenderableObjectHandle& other) const
{
	return this->m_id == other.m_id;
}

bool RenderableObjectHandle::operator != (const RenderableObjectHandle& other) const
{
	return !(*this == other);
}

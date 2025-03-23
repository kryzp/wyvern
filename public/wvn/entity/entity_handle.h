#ifndef ENTITY_H_ANDLE_H_
#define ENTITY_H_ANDLE_H_

#include <wvn/entity/entity_common.h>

namespace wvn::ent
{
	class Entity;

	class EntityHandle
	{
	public:
		EntityHandle();
		EntityHandle(const Entity* entity);
		EntityHandle(EntityID id);
		EntityHandle(const EntityHandle& other);
		EntityHandle(EntityHandle&& other) noexcept;
		EntityHandle& operator = (const EntityHandle& other);
		EntityHandle& operator = (EntityHandle&& other) noexcept;
		~EntityHandle() = default;

		bool is_valid() const;
		operator bool () const;

		EntityID id() const;
		operator EntityID () const;

		Entity* get();
		const Entity* get() const;

		template <typename T> T* get() { return (T*)get(); }
		template <typename T> const T* get() const { return (const T*)get(); }

		Entity* operator -> ();
		const Entity* operator -> () const;

		bool operator == (const EntityHandle& other) const;
		bool operator != (const EntityHandle& other) const;

	private:
		EntityID m_id;
	};
}

#endif // ENTITY_H_ANDLE_H_

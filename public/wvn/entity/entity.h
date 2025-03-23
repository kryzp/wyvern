#ifndef ENTITY_H_
#define ENTITY_H_

#include <wvn/entity/entity_common.h>
#include <wvn/maths/transform_3d.h>
#include <wvn/graphics/renderable_object.h>

namespace wvn::phys { class RigidBody; }

namespace wvn::ent
{
	class EntityHandle;
	class EntityMgr;
	class Event;

	/**
	 * Base class for all entities in the game world.
	 */
	class Entity
	{
		friend class EntityHandle;
		friend class EntityMgr;

	public:
		constexpr static EntityID NULL_ID = 0;

		Entity();
		virtual ~Entity() = default;

		// should these functions should have clearer names like on_init and on_destroy?
		virtual void init() { }
		virtual void destroy() { }

		virtual void enter() { }
		virtual void exit() { }

		virtual void tick();

		Transform3D& get_transform();
		gfx::RenderableObjectHandle get_renderable_object();

		void add_flag(u64 flag);
		void remove_flag(u64 flag);
		void toggle_flag(u64 flag);
		void set_flag(u64 flag, bool mode);
		bool has_flag(u64 flag) const;
		bool only_has_flag(u64 flag) const;
		void clear_flags();
		u64 flags() const;

		EntityID id() const;

		virtual bool on_event(const Event& e);

	protected:
		Transform3D m_transform;
		phys::RigidBody* m_rigidbody;
		gfx::RenderableObjectHandle m_renderable_object;

		Vec3F m_velocity;
		Vec3F m_angular_velocity;

	private:
		u64 m_flags;
		EntityID m_id;
	};
}

#endif // ENTITY_H_

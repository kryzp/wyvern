#ifndef ACTOR_H
#define ACTOR_H

#include <wvn/common.h>
#include <wvn/maths/transform_3d.h>

namespace wvn::gfx { class Model; }
namespace wvn::phys { class Rigidbody; }

namespace wvn::act
{
	class ActorHandle;
	class ActorMgr;
	class Event;

	/**
	 * Base class for all actors in the game world.
	 */
	class Actor
	{
		friend class ActorHandle;
		friend class ActorMgr;

	public:
		constexpr static u32 NULL_ID = 0;

		Actor();
		virtual ~Actor() = default;

		// should these functions should have clearer names like on_init and on_destroy?
		virtual void init() { }
		virtual void destroy() { }

		virtual void enter() { }
		virtual void exit() { }

		virtual void tick();

		Transform3D& get_transform();
		gfx::Model* get_model();

		void add_flag(u64 flag);
		void remove_flag(u64 flag);
		void toggle_flag(u64 flag);
		void set_flag(u64 flag, bool mode);
		bool has_flag(u64 flag) const;
		bool only_has_flag(u64 flag) const;
		void clear_flags();
		u64 flags() const;

		u32 id() const;

		virtual bool on_event(Event& e);

	protected:
		Transform3D p_transform;
		phys::Rigidbody* p_rigidbody;
		gfx::Model* p_model;

		Vec3F p_velocity;
		Vec3F p_angular_velocity;

	private:
		u64 m_flags;
		u32 m_id;
	};
}

#endif // ACTOR_H

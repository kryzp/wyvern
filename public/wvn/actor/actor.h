#pragma once

#include <wvn/util/types.h>

namespace wvn::act
{
	class ActorHandle;
	class ActorMgr;
	class Event;

	using ActorID = u32;

	/*
	 * Base class for all actors in the game world.
	 */
	class Actor
	{
		friend class ActorHandle;

		// todo: this will become friend class Scene once scenes are properly implemented
		friend class ActorMgr;

	public:
		constexpr static ActorID INVALID_ID = 0;

		Actor();
		virtual ~Actor() = default;

		virtual void init() { }
		virtual void destroy() { }

		virtual void enter() { }
		virtual void exit() { }

		virtual void tick() { }

		virtual bool on_event(Event& e);

		void add_flag(u64 flag);
		void remove_flag(u64 flag);
		void toggle_flag(u64 flag);
		void set_flag(u64 flag, bool mode);
		bool has_flag(u64 flag) const;
		bool only_has_flag(u64 flag) const;
		void clear_flags();

	private:
		u64 m_flags;
		ActorID m_id;
	};

	class ActorHandle
	{
	public:
		ActorHandle();
		ActorHandle(const Actor* actor);
		ActorHandle(ActorID id);
		ActorHandle(const ActorHandle& other);
		ActorHandle(ActorHandle&& other) noexcept;
		ActorHandle& operator = (const ActorHandle& other);
		ActorHandle& operator = (ActorHandle&& other) noexcept;
		~ActorHandle() = default;

		bool is_valid() const;
		operator bool () const;

		ActorID id() const;
		operator ActorID () const;

		Actor* get();
		const Actor* get() const;

		Actor* operator -> ();
		const Actor* operator -> () const;

		bool operator == (const ActorHandle& other) const;
		bool operator != (const ActorHandle& other) const;

	private:
		ActorID m_id;
	};
}

#pragma once

#include <wvn/actor/event.h>
#include <wvn/util/types.h>

namespace wvn
{
	class ActorHandle;
	class ActorMgr;

	using ActorID = u32;
	using ActorFlag = u32;

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

		virtual bool on_event(Event& e) { return false; }

		void add_flag(ActorFlag flag);
		void remove_flag(ActorFlag flag);
		bool has_flag(ActorFlag flag) const;
		bool only_has_flag(ActorFlag flag) const;
		void clear_flags();

	private:
		ActorFlag m_flags;
		ActorID m_id;
	};

	class ActorHandle
	{
	public:
		ActorHandle();
		ActorHandle(Actor* actor);
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

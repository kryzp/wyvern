#ifndef ACTOR_HANDLE_H
#define ACTOR_HANDLE_H

#include <wvn/common.h>

namespace wvn::act
{
	class Actor;

	class ActorHandle
	{
	public:
		ActorHandle();
		ActorHandle(const Actor* actor);
		ActorHandle(u32 id);
		ActorHandle(const ActorHandle& other);
		ActorHandle(ActorHandle&& other) noexcept;
		ActorHandle& operator = (const ActorHandle& other);
		ActorHandle& operator = (ActorHandle&& other) noexcept;
		~ActorHandle() = default;

		bool is_valid() const;
		operator bool () const;

		u32 id() const;
		operator u32 () const;

		Actor* get();
		const Actor* get() const;

		Actor* operator -> ();
		const Actor* operator -> () const;

		bool operator == (const ActorHandle& other) const;
		bool operator != (const ActorHandle& other) const;

	private:
		u32 m_id;
	};
}

#endif // ACTOR_HANDLE_H

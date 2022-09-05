#pragma once

#include <wvn/actor/event.h>

namespace wvn
{
	class Actor
	{
	public:
		Actor() = default;
		~Actor() = default;

		virtual void init() { }
		virtual void destroy() { }

		virtual void enter() { }
		virtual void exit() { }

		virtual void tick() { }

		virtual bool on_event(Event& e) { return false; }
	};
}

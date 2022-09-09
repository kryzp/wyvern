#pragma once

#include <wvn/util/singleton.h>
#include <wvn/util/types.h>

#include <wvn/math/vec2.h>
#include <wvn/container/string.h>

namespace wvn::sys
{
	// not really a "mgr" more like a wrapper to make accessing window stuff easier
	// probably not necessary :/
	class WindowMgr : public Singleton<WindowMgr>
	{
		WVN_DEF_SINGLETON(WindowMgr)

	public:
		WindowMgr();
		~WindowMgr();

		String get_window_name() const;
		void set_window_name(const String& name) const;

		Vec2I get_window_position() const;
		void set_window_position(const Vec2I& position) const;

		Vec2U get_window_size() const;
		void set_window_size(const Vec2U& size) const;

		Vec2U get_screen_size() const;

		void show_cursor() const;
		void hide_cursor() const;
		void toggle_cursor_visible(bool toggle) const;
		bool is_cursor_visible() const;

		void sleep_for(u64 ms) const;
		u64 ticks() const;
	};
}

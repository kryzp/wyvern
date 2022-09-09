#include <wvn/system/window_mgr.h>
#include <wvn/system/system_backend.h>
#include <wvn/root.h>

using namespace wvn;
using namespace wvn::sys;

WVN_IMPL_SINGLETON(WindowMgr);

WindowMgr::WindowMgr()
{
	const auto config = Root::get_singleton().config();
	const auto window = get_window_size();
	const auto screen = get_screen_size();

	// set window name
	set_window_name(String(config.name));

	// apply window size
	set_window_size(Vec2U(config.width, config.height));

	// centre the window
	set_window_position(Vec2I(
		(screen.width / 2) - (window.width / 2),
		(screen.height / 2) - (window.height / 2)
	));

	// set cursor visible
	if (config.has_flag(Config::FLAG_CURSOR_VISIBLE)) {
		show_cursor();
	} else {
		hide_cursor();
	}
}

WindowMgr::~WindowMgr()
{
}

String WindowMgr::get_window_name() const
{
	return Root::get_singleton().current_system_backend()->get_window_name();
}

void WindowMgr::set_window_name(const wvn::String& name) const
{
	Root::get_singleton().current_system_backend()->set_window_name(name);
}

Vec2I WindowMgr::get_window_position() const
{
	return Root::get_singleton().current_system_backend()->get_window_position();
}

void WindowMgr::set_window_position(const Vec2I& position) const
{
	Root::get_singleton().current_system_backend()->set_window_position(position);
}

Vec2U WindowMgr::get_window_size() const
{
	return Root::get_singleton().current_system_backend()->get_window_size();
}

void WindowMgr::set_window_size(const Vec2U& size) const
{
	Root::get_singleton().current_system_backend()->set_window_size(size);
}

Vec2U WindowMgr::get_screen_size() const
{
	return Root::get_singleton().current_system_backend()->get_screen_size();
}

void WindowMgr::show_cursor() const
{
	Root::get_singleton().current_system_backend()->toggle_cursor_visible(true);
}

void WindowMgr::hide_cursor() const
{
	Root::get_singleton().current_system_backend()->toggle_cursor_visible(false);
}

void WindowMgr::toggle_cursor_visible(bool toggle) const
{
	Root::get_singleton().current_system_backend()->toggle_cursor_visible(toggle);
}

bool WindowMgr::is_cursor_visible() const
{
	return Root::get_singleton().current_system_backend()->is_cursor_visible();
}

void WindowMgr::sleep_for(u64 ms) const
{
	Root::get_singleton().current_system_backend()->sleep_for(ms);
}

u64 WindowMgr::ticks() const
{
	return Root::get_singleton().current_system_backend()->ticks();
}

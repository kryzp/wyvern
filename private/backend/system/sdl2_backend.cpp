#include <backend/system/sdl2_backend.h>

using namespace wvn;
using namespace wvn::sys;

SDL2Backend::SDL2Backend()
{
}

SDL2Backend::~SDL2Backend()
{
}

SystemProperties SDL2Backend::properties()
{
	SystemProperties properties = {};
	properties.temp = false;

	return properties;
}

String SDL2Backend::get_window_name()
{
}

void SDL2Backend::set_window_name(const String& name)
{
}

Vec2I SDL2Backend::get_window_position()
{
}

void SDL2Backend::set_window_position(const Vec2I& position)
{
}

Vec2U SDL2Backend::get_window_size()
{
}

void SDL2Backend::set_window_size(const Vec2U& size)
{
}

Vec2U SDL2Backend::get_screen_size()
{
}

void SDL2Backend::toggle_cursor_visible(bool toggle)
{
}

bool SDL2Backend::is_cursor_visible()
{
}

void SDL2Backend::sleep_for(u64 ms)
{
}

u64 SDL2Backend::ticks()
{
}

void* SDL2Backend::stream_from_file(const char* filepath, const char* mode)
{
}

void* SDL2Backend::stream_from_memory(void* memory, s64 size)
{
}

void* SDL2Backend::stream_from_const_memory(const void* memory, s64 size)
{
}

s64 SDL2Backend::stream_read(void* stream, void* ptr, s64 size)
{
}

s64 SDL2Backend::stream_write(void* stream, const void* ptr, s64 size)
{
}

s64 SDL2Backend::stream_seek(void* stream, s64 offset)
{
}

s64 SDL2Backend::stream_size(void* stream)
{
}

s64 SDL2Backend::stream_position(void* stream)
{
}

void SDL2Backend::stream_close(void* stream)
{
}

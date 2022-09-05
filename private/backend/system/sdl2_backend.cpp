#include <backend/system/sdl2_backend.h>

using namespace wvn;
using namespace wvn::bknd;

SDL2Backend::SDL2Backend()
{
}

SDL2Backend::~SDL2Backend()
{
}

SystemProperties SDL2Backend::properties()
{
}

void SDL2Backend::update()
{
}

void SDL2Backend::present()
{
}

void SDL2Backend::set_window_position(int x, int y)
{
}

void SDL2Backend::get_window_position(int* x, int* y)
{
}

int SDL2Backend::window_width()
{
}

int SDL2Backend::window_height()
{
}

int SDL2Backend::draw_width()
{
}

int SDL2Backend::draw_height()
{
}

void SDL2Backend::show_cursor(bool toggle)
{
}

bool SDL2Backend::cursor_visible()
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

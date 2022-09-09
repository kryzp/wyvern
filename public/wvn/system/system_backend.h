#pragma once

#include <wvn/util/types.h>
#include <wvn/math/vec2.h>
#include <wvn/container/string.h>

namespace wvn::sys
{
	struct SystemProperties
	{
		bool temp;
	};

	class SystemBackend
	{
	public:
		SystemBackend() = default;
		virtual ~SystemBackend() = default;

		virtual SystemProperties properties() = 0;

		virtual String get_window_name() = 0;
		virtual void set_window_name(const String& name) = 0;

		virtual Vec2I get_window_position() = 0;
		virtual void set_window_position(const Vec2I& position) = 0;

		virtual Vec2U get_window_size() = 0;
		virtual void set_window_size(const Vec2U& size) = 0;

		virtual Vec2U get_screen_size() = 0;

		virtual void toggle_cursor_visible(bool toggle) = 0;
		virtual bool is_cursor_visible() = 0;

		virtual void sleep_for(u64 ms) = 0;
		virtual u64 ticks() = 0;

		virtual void* stream_from_file(const char* filepath, const char* mode) = 0;
		virtual void* stream_from_memory(void* memory, s64 size) = 0;
		virtual void* stream_from_const_memory(const void* memory, s64 size) = 0;
		virtual s64 stream_read(void* stream, void* ptr, s64 size) = 0;
		virtual s64 stream_write(void* stream, const void* ptr, s64 size) = 0;
		virtual s64 stream_seek(void* stream, s64 offset) = 0;
		virtual s64 stream_size(void* stream) = 0;
		virtual s64 stream_position(void* stream) = 0;
		virtual void stream_close(void* stream) = 0;
	};
}

#pragma once

#include <wvn/util/types.h>

namespace wvn::bknd
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

		virtual void update() = 0;
		virtual void present() = 0;

		virtual void set_window_position(int x, int y) = 0;
		virtual void get_window_position(int* x, int* y) = 0;

		virtual int window_width() = 0;
		virtual int window_height() = 0;

		virtual int draw_width() = 0;
		virtual int draw_height() = 0;

		virtual void show_cursor(bool toggle) = 0;
		virtual bool cursor_visible() = 0;

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

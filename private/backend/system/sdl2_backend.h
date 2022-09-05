#pragma once

#include <wvn/system/system_backend.h>

namespace wvn::bknd
{
	class SDL2Backend : public SystemBackend
	{
	public:
		SDL2Backend();
		~SDL2Backend() override;

		SystemProperties properties() override;

		void update() override;
		void present() override;

		void set_window_position(int x, int y) override;
		void get_window_position(int* x, int* y) override;

		int window_width() override;
		int window_height() override;

		int draw_width() override;
		int draw_height() override;

		void show_cursor(bool toggle) override;
		bool cursor_visible() override;

		void sleep_for(u64 ms) override;
		u64 ticks() override;

		void* stream_from_file(const char* filepath, const char* mode) override;
		void* stream_from_memory(void* memory, s64 size) override;
		void* stream_from_const_memory(const void* memory, s64 size) override;
		s64 stream_read(void* stream, void* ptr, s64 size) override;
		s64 stream_write(void* stream, const void* ptr, s64 size) override;
		s64 stream_seek(void* stream, s64 offset) override;
		s64 stream_size(void* stream) override;
		s64 stream_position(void* stream) override;
		void stream_close(void* stream) override;
	};
}

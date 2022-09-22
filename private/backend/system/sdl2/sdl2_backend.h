#pragma once

#include <wvn/system/system_backend.h>
#include <SDL.h>

namespace wvn::sys
{
	class SDL2Backend : public SystemBackend
	{
	public:
		SDL2Backend();
		~SDL2Backend() override;

		SystemProperties properties() override;

		void poll_events() override;

		String get_window_name() override;
		void set_window_name(const String& name) override;

		Vec2I get_window_position() override;
		void set_window_position(const Vec2I& position) override;

		Vec2I get_window_size() override;
		void set_window_size(const Vec2I& size) override;

		Vec2I get_screen_size() override;

		void toggle_cursor_visible(bool toggle) override;
		bool is_cursor_visible() override;

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

		// vulkan specific
		bool vk_get_instance_extensions(u32* count, const char** names) override;

	private:
		SDL_Window* m_window;
	};
}

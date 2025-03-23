#ifndef SDL3_BACKEND_H_
#define SDL3_BACKEND_H_

#include <wvn/system/system_backend.h>
#include <wvn/container/array.h>
#include <SDL.h>

namespace wvn::sys
{
	class SDL3Backend : public SystemBackend
	{
	public:
		SDL3Backend();
		~SDL3Backend() override;

		SystemBackendProperties properties() override;

		void poll_events() override;

		String get_window_name() override;
		void set_window_name(const String& name) override;

		Vec2I get_window_position() override;
		void set_window_position(const Vec2I& position) override;

		Vec2I get_window_size() override;
		void set_window_size(const Vec2I& size) override;

		Vec2I get_screen_size() override;

		float get_window_opacity() override;
		void set_window_opacity(float opacity) override;

		bool is_window_resizable() override;
		void toggle_window_resizable(bool toggle) override;

		float get_window_refresh_rate() override;
		float get_window_pixel_density() override;

		bool is_cursor_visible() override;
		void toggle_cursor_visible(bool toggle) override;
		void lock_cursor(bool toggle) override;
		void set_cursor_position(int x, int y) override;

		void reconnect_all_gamepads() override;
		inp::GamepadType get_gamepad_type(int player) override;
		void close_gamepad(int player) override;

		WindowMode get_window_mode() override;
		void set_window_mode(WindowMode toggle) override;

		void sleep_for(u64 ms) override;
		u64 ticks() override;

		void* stream_from_file(const char* filepath, const char* mode) override;
		void* stream_from_memory(void* memory, u64 size) override;
		void* stream_from_const_memory(const void* memory, u64 size) override;
		s64 stream_read(void* stream, void* ptr, u64 size) override;
		s64 stream_write(void* stream, const void* ptr, u64 size) override;
		s64 stream_seek(void* stream, s64 offset) override;
		s64 stream_size(void* stream) override;
		s64 stream_position(void* stream) override;
		void stream_close(void* stream) override;

#if wvn_USE_VULKAN
		const char* const* vk_get_instance_extensions(u32* count) override;
		bool vk_create_surface(VkInstance instance, VkSurfaceKHR* surface) override;
#endif // wvn_USE_VULKAN

	private:
		void close_all_gamepads();

		SDL_Window* m_window;
		Array<SDL_Gamepad*, inp::MAX_GAMEPADS> m_gamepads;
		int m_gamepad_count;
	};
}

#endif // SDL3_BACKEND_H_

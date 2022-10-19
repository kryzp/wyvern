#pragma once

#include <wvn/util/types.h>
#include <wvn/maths/vec2.h>
#include <wvn/container/string.h>
#include <wvn/root.h>

#if WVN_USE_VULKAN
class VkInstance_T;
using VkInstance = VkInstance_T*;

class VkSurfaceKHR_T;
using VkSurfaceKHR = VkSurfaceKHR_T*;
#endif

namespace wvn::sys
{
	/*
	 * Properties a system backend might have that are exclusive to itself.
	 */
	struct SystemBackendProperties
	{
		bool temp;
	};

	/*
	 * System backend that can be implemented by specific API's to
	 * disconnect them from being directly connected to the actual game
	 * engine, effectively acting as an interface.
	 */
	class SystemBackend
	{
	public:
		SystemBackend() = default;
		virtual ~SystemBackend() = default;

		virtual SystemBackendProperties properties() = 0;

		virtual void poll_events() = 0;

		virtual String get_window_name() = 0;
		virtual void set_window_name(const String& name) = 0;

		virtual Vec2I get_window_position() = 0;
		virtual void set_window_position(const Vec2I& position) = 0;

		virtual Vec2I get_window_size() = 0;
		virtual void set_window_size(const Vec2I& size) = 0;

		virtual Vec2I get_draw_size() = 0;
		virtual Vec2I get_screen_size() = 0;

		virtual float get_window_opacity() = 0;
		virtual void set_window_opacity(float opacity) = 0;

		virtual bool is_window_resizable() = 0;
		virtual void toggle_window_resizable(bool toggle) = 0;

		virtual bool is_cursor_visible() = 0;
		virtual void toggle_cursor_visible(bool toggle) = 0;

		virtual WindowMode get_window_mode() = 0;
		virtual void set_window_mode(WindowMode toggle) = 0;

		virtual void sleep_for(u64 ms) = 0;
		virtual u64 ticks() = 0;

		virtual void* stream_from_file(const char* filepath, const char* mode) = 0;
		virtual void* stream_from_memory(void* memory, u64 size) = 0;
		virtual void* stream_from_const_memory(const void* memory, u64 size) = 0;
		virtual s64 stream_read(void* stream, void* ptr, u64 size) = 0;
		virtual s64 stream_write(void* stream, const void* ptr, u64 size) = 0;
		virtual s64 stream_seek(void* stream, s64 offset) = 0;
		virtual s64 stream_size(void* stream) = 0;
		virtual s64 stream_position(void* stream) = 0;
		virtual void stream_close(void* stream) = 0;

#if WVN_USE_VULKAN
		virtual bool vk_get_instance_extensions(u32* count, const char** names) = 0;
		virtual bool vk_create_surface(VkInstance instance, VkSurfaceKHR* surface) = 0;
#endif
	};
}

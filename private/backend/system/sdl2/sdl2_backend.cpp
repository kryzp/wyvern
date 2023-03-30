#include <backend/system/sdl2/sdl2_backend.h>

#include <wvn/root.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/graphics/renderer_backend.h>
#include <wvn/input/input_mgr.h>

#if WVN_USE_VULKAN
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#endif

#if WVN_USE_OPENGL
#include <SDL_opengl.h>
#endif

using namespace wvn;
using namespace wvn::sys;

SDL2Backend::SDL2Backend()
	: m_window(nullptr)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		dev::LogMgr::get_singleton()->print("[SDL2] %s", SDL_GetError());
		WVN_ERROR("[SDL2|DEBUG] Failed to initialize!");
	}

	u64 flags = SDL_WINDOW_ALLOW_HIGHDPI;

	const auto& cfg = Root::get_singleton()->config();

	if (cfg.has_flag(Config::FLAG_RESIZABLE)) {
		flags |= SDL_WINDOW_RESIZABLE;
	}

#ifdef WVN_USE_VULKAN
	flags |= SDL_WINDOW_VULKAN;
#endif

	m_window = SDL_CreateWindow(cfg.name, 0, 0, cfg.width, cfg.height, flags);

	if (!m_window) {
		WVN_ERROR("[SDL2|DEBUG] Failed to create window.");
	}

	dev::LogMgr::get_singleton()->print("[SDL2] Initialized!");
}

SDL2Backend::~SDL2Backend()
{
	SDL_DestroyWindow(m_window);
	SDL_Quit();

	dev::LogMgr::get_singleton()->print("[SDL2] Destroyed!");
}

SystemBackendProperties SDL2Backend::properties()
{
	SystemBackendProperties properties = {};
	properties.temp = false;

	return properties;
}

void SDL2Backend::poll_events()
{
	SDL_Event e;
	inp::JoystickHat v0;

	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
			case SDL_QUIT:
				Root::get_singleton()->exit();
				break;

			case SDL_WINDOWEVENT_RESIZED:
				Root::get_singleton()->renderer_backend()->on_window_resize(e.window.data1, e.window.data2);
				break;

			case SDL_MOUSEWHEEL:
				inp::InputMgr::get_singleton()->on_mouse_wheel(e.wheel.x, e.wheel.y);
				break;

			case SDL_MOUSEMOTION:
				int spx, spy;
				SDL_GetGlobalMouseState(&spx, &spy);
				inp::InputMgr::get_singleton()->on_mouse_screen_move(spx, spy);
				inp::InputMgr::get_singleton()->on_mouse_move(e.motion.x, e.motion.y);
				break;

			case SDL_MOUSEBUTTONDOWN:
				inp::InputMgr::get_singleton()->on_mouse_down(e.button.button);
				break;

			case SDL_MOUSEBUTTONUP:
				inp::InputMgr::get_singleton()->on_mouse_up(e.button.button);
				break;

			case SDL_KEYDOWN:
				inp::InputMgr::get_singleton()->on_key_down(e.key.keysym.scancode);
				break;

			case SDL_KEYUP:
				inp::InputMgr::get_singleton()->on_key_up(e.key.keysym.scancode);
				break;

			case SDL_TEXTINPUT:
				inp::InputMgr::get_singleton()->on_text_utf8(e.text.text);
				break;

			case SDL_JOYBUTTONDOWN:
				inp::InputMgr::get_singleton()->on_joystick_button_down(e.jball.which, e.jbutton.button);
				break;

			case SDL_JOYBUTTONUP:
				inp::InputMgr::get_singleton()->on_joystick_button_up(e.jball.which, e.jbutton.button);
				break;

			case SDL_JOYAXISMOTION:
				inp::InputMgr::get_singleton()->on_joystick_motion(
					e.jaxis.which,
					(e.jaxis.axis == 0) ? inp::JS_AXIS_H : inp::JS_AXIS_V,
					static_cast<float>(e.jaxis.value) / static_cast<float>(SDL_JOYSTICK_AXIS_MAX - ((e.jaxis.value >= 0) ? 1.0f : 0.0f))
				);
				break;

			default:
				break;
		}
	}
}

String SDL2Backend::get_window_name()
{
	String name = SDL_GetWindowTitle(m_window);
	return name;
}

void SDL2Backend::set_window_name(const String& name)
{
	SDL_SetWindowTitle(m_window, name.c_str());
}

Vec2I SDL2Backend::get_window_position()
{
	Vec2I result = Vec2I::zero();
	SDL_GetWindowPosition(m_window, &result.x, &result.y);
	return result;
}

void SDL2Backend::set_window_position(const Vec2I& position)
{
	SDL_SetWindowPosition(m_window, position.x, position.y);
}

Vec2I SDL2Backend::get_window_size()
{
	Vec2I result = Vec2I::zero();
	SDL_GetWindowSize(m_window, &result.x, &result.y);
	return result;
}

void SDL2Backend::set_window_size(const Vec2I& size)
{
	SDL_SetWindowSize(m_window, size.x, size.y);
}

Vec2I SDL2Backend::get_draw_size()
{
	Vec2I result = Vec2I::zero();

#if WVN_USE_VULKAN
	SDL_Vulkan_GetDrawableSize(m_window, &result.x, &result.y);
#endif

	return result;
}

Vec2I SDL2Backend::get_screen_size()
{
	SDL_DisplayMode display_mode = {};
	SDL_GetCurrentDisplayMode(0, &display_mode);
	return { display_mode.w, display_mode.h };
}

float SDL2Backend::get_window_opacity()
{
	float result = 1.0f;
	SDL_GetWindowOpacity(m_window, &result);
	return result;
}

void SDL2Backend::set_window_opacity(float opacity)
{
	SDL_SetWindowOpacity(m_window, opacity);
}

bool SDL2Backend::is_window_resizable()
{
	return SDL_GetWindowFlags(m_window) & SDL_WINDOW_RESIZABLE;
}

void SDL2Backend::toggle_window_resizable(bool toggle)
{
	SDL_SetWindowResizable(m_window, static_cast<SDL_bool>(toggle));
}

bool SDL2Backend::is_cursor_visible()
{
	return SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE;
}

void SDL2Backend::toggle_cursor_visible(bool toggle)
{
	SDL_ShowCursor(toggle ? SDL_ENABLE : SDL_DISABLE);
}

WindowMode SDL2Backend::get_window_mode()
{
	auto flags = SDL_GetWindowFlags(m_window);

	if (flags & SDL_WINDOW_FULLSCREEN) {
		return WINDOW_MODE_FULLSCREEN;
	} else if (flags & SDL_WINDOW_BORDERLESS) {
		return WINDOW_MODE_BORDERLESS;
	}

	return WINDOW_MODE_WINDOWED;
}

void SDL2Backend::set_window_mode(WindowMode toggle)
{
	switch (toggle)
	{
		case WINDOW_MODE_FULLSCREEN:
			SDL_SetWindowFullscreen(m_window, true);
			break;

		case WINDOW_MODE_BORDERLESS:
			SDL_SetWindowFullscreen(m_window, false);
			SDL_SetWindowBordered(m_window, SDL_FALSE);
			break;

		case WINDOW_MODE_WINDOWED:
			SDL_SetWindowFullscreen(m_window, false);
			SDL_SetWindowBordered(m_window, SDL_TRUE);
	}
}

void SDL2Backend::sleep_for(u64 ms)
{
	if (ms > 0) {
		SDL_Delay(ms);
	}
}

u64 SDL2Backend::ticks()
{
	return SDL_GetTicks64();
}

void* SDL2Backend::stream_from_file(const char* filepath, const char* mode)
{
	return SDL_RWFromFile(filepath, mode);
}

void* SDL2Backend::stream_from_memory(void* memory, u64 size)
{
	return SDL_RWFromMem(memory, size);
}

void* SDL2Backend::stream_from_const_memory(const void* memory, u64 size)
{
	return SDL_RWFromConstMem(memory, size);
}

s64 SDL2Backend::stream_read(void* stream, void* ptr, u64 size)
{
	return SDL_RWread((SDL_RWops*)stream, ptr, sizeof(char), size);
}

s64 SDL2Backend::stream_write(void* stream, const void* ptr, u64 size)
{
	return SDL_RWwrite((SDL_RWops*)stream, ptr, sizeof(char), size);
}

s64 SDL2Backend::stream_seek(void* stream, s64 offset)
{
	return SDL_RWseek((SDL_RWops*)stream, offset, RW_SEEK_SET);
}

s64 SDL2Backend::stream_size(void* stream)
{
	return SDL_RWsize((SDL_RWops*)stream);
}

s64 SDL2Backend::stream_position(void* stream)
{
	return SDL_RWtell((SDL_RWops*)stream);
}

void SDL2Backend::stream_close(void* stream)
{
	SDL_RWclose((SDL_RWops*)stream);
}

#if WVN_USE_VULKAN

bool SDL2Backend::vk_get_instance_extensions(u32* count, const char** names)
{
	return SDL_Vulkan_GetInstanceExtensions(m_window, count, names);
}

bool SDL2Backend::vk_create_surface(VkInstance instance, VkSurfaceKHR* surface)
{
	return SDL_Vulkan_CreateSurface(m_window, instance, surface);
}

#endif

#include <backend/system/sdl3/sdl3_backend.h>

#include <wvn/root.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/graphics/renderer_backend.h>
#include <wvn/input/input.h>

#if wvn_USE_VULKAN
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#endif // wvn_USE_VULKAN

#if wvn_USE_OPENGL
#include <SDL_opengl.h>
#endif // wvn_USE_OPENGL

using namespace wvn;
using namespace wvn::sys;

SDL3Backend::SDL3Backend()
	: m_window(nullptr)
	, m_gamepads{}
	, m_gamepad_count(0)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		wvn_ERROR("[SDL3|DEBUG] Failed to initialize: %s", SDL_GetError());
	}

	u64 flags = SDL_WINDOW_HIGH_PIXEL_DENSITY;

	const auto& cfg = Root::get_singleton()->config();

	if (cfg.has_flag(Config::FLAG_RESIZABLE)) {
		flags |= SDL_WINDOW_RESIZABLE;
	}

#ifdef wvn_USE_VULKAN
	flags |= SDL_WINDOW_VULKAN;
#endif // wvn_USE_VULKAN

	m_window = SDL_CreateWindow(cfg.name, cfg.width, cfg.height, flags);

	if (!m_window) {
		wvn_ERROR("[SDL3|DEBUG] Failed to create window.");
	}

	dev::LogMgr::get_singleton()->print("[SDL3] Initialized!");
}

SDL3Backend::~SDL3Backend()
{
	close_all_gamepads();

	SDL_DestroyWindow(m_window);
	SDL_Quit();

	dev::LogMgr::get_singleton()->print("[SDL3] Destroyed!");
}

SystemBackendProperties SDL3Backend::properties()
{
	SystemBackendProperties properties = {};
	properties.temp = false; // todo: remove this
	return properties;
}

void SDL3Backend::poll_events()
{
	float spx = 0.f, spy = 0.f;
	SDL_Event e = {};

	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_EVENT_QUIT:
			dev::LogMgr::get_singleton()->print("[SDL3] Detected window close event, quitting...");
			Root::get_singleton()->exit();
			break;

		case SDL_EVENT_WINDOW_RESIZED:
			dev::LogMgr::get_singleton()->print("[SDL3] Detected window resize!");
			Root::get_singleton()->renderer_backend()->on_window_resize(e.window.data1, e.window.data2);
			break;

		case SDL_EVENT_MOUSE_WHEEL:
			inp::Input::get_singleton()->on_mouse_wheel(e.wheel.x, e.wheel.y);
			break;

		case SDL_EVENT_MOUSE_MOTION:
			SDL_GetGlobalMouseState(&spx, &spy);
			inp::Input::get_singleton()->on_mouse_screen_move(spx, spy);
			inp::Input::get_singleton()->on_mouse_move(e.motion.x, e.motion.y);
			break;

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			inp::Input::get_singleton()->on_mouse_down(e.button.button);
			break;

		case SDL_EVENT_MOUSE_BUTTON_UP:
			inp::Input::get_singleton()->on_mouse_up(e.button.button);
			break;

		case SDL_EVENT_KEY_DOWN:
			inp::Input::get_singleton()->on_key_down(e.key.keysym.scancode);
			break;

		case SDL_EVENT_KEY_UP:
			inp::Input::get_singleton()->on_key_up(e.key.keysym.scancode);
			break;

		case SDL_EVENT_TEXT_INPUT:
			inp::Input::get_singleton()->on_text_utf8(e.text.text);
			break;

		case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
			inp::Input::get_singleton()->on_gamepad_button_down(e.gbutton.button, SDL_GetGamepadInstancePlayerIndex(e.gbutton.which));
			break;

		case SDL_EVENT_GAMEPAD_BUTTON_UP:
			inp::Input::get_singleton()->on_gamepad_button_up(e.gbutton.button, SDL_GetGamepadInstancePlayerIndex(e.gbutton.which));
			break;

		case SDL_EVENT_GAMEPAD_AXIS_MOTION:
			inp::Input::get_singleton()->on_gamepad_motion(
				SDL_GetGamepadInstancePlayerIndex(e.gaxis.which),
				(inp::GamepadAxis)e.gaxis.axis,
				(float)(e.gaxis.value) / (float)(SDL_JOYSTICK_AXIS_MAX - ((e.gaxis.value >= 0) ? 1.0f : 0.0f))
			);
			break;

		case SDL_EVENT_GAMEPAD_ADDED:
			dev::LogMgr::get_singleton()->print("[SDL3] Gamepad added, trying to reconnect all gamepads...");
			reconnect_all_gamepads();
			break;

		case SDL_EVENT_GAMEPAD_REMOVED:
			dev::LogMgr::get_singleton()->print("[SDL3] Gamepad removed.");
			break;

		case SDL_EVENT_GAMEPAD_REMAPPED:
			dev::LogMgr::get_singleton()->print("[SDL3] Gamepad remapped.");
			break;

		case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
			break;

		case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
			break;

		case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
			break;

		case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
			break;

		case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
			break;

		default:
			break;
		}
	}
}

void SDL3Backend::reconnect_all_gamepads()
{
	if (m_gamepads[0] != nullptr) {
		close_all_gamepads();
	}

	SDL_JoystickID* gamepad_ids = SDL_GetGamepads(&m_gamepad_count);

	if (m_gamepad_count == 0)
	{
		dev::LogMgr::get_singleton()->print("[SDL3] No gamepads found!");
		goto finished;
	}
	else
	{
		dev::LogMgr::get_singleton()->print("[SDL3] Found %d gamepads!", m_gamepad_count);
	}

	for (int i = 0; i < m_gamepad_count; i++)
	{
		SDL_JoystickID id = gamepad_ids[i];
		m_gamepads[i] = SDL_OpenGamepad(id);

		if (m_gamepads[i]) {
			dev::LogMgr::get_singleton()->print("[SDL3] Opened gamepad with id: %d, internal index: %d, and player index: %d.", id, i,
				SDL_GetGamepadPlayerIndex(m_gamepads[i])
			);
		} else {
			dev::LogMgr::get_singleton()->print("[SDL3] Failed to open gamepad with id: %d, and internal index: %d.", id, i);
		}
	}

finished:
	SDL_free(gamepad_ids);
}

void SDL3Backend::close_all_gamepads()
{
	for (int i = 0; i < m_gamepad_count; i++) {
		SDL_CloseGamepad(m_gamepads[i]);
		dev::LogMgr::get_singleton()->print("[SDL3] Closed gamepad with internal index %d.", i);
	}

	m_gamepad_count = 0;
	m_gamepads.clear();
}

inp::GamepadType SDL3Backend::get_gamepad_type(int player)
{
	return (inp::GamepadType)SDL_GetGamepadType(SDL_GetGamepadFromPlayerIndex(player));
}

void SDL3Backend::close_gamepad(int player)
{
	SDL_CloseGamepad(SDL_GetGamepadFromPlayerIndex(player));
}

String SDL3Backend::get_window_name()
{
	String name = SDL_GetWindowTitle(m_window);
	return name;
}

void SDL3Backend::set_window_name(const String& name)
{
	SDL_SetWindowTitle(m_window, name.c_str());
}

Vec2I SDL3Backend::get_window_position()
{
	Vec2I result = Vec2I::zero();
	SDL_GetWindowPosition(m_window, &result.x, &result.y);
	return result;
}

void SDL3Backend::set_window_position(const Vec2I& position)
{
	SDL_SetWindowPosition(m_window, position.x, position.y);
}

Vec2I SDL3Backend::get_window_size()
{
	Vec2I result = Vec2I::zero();
	SDL_GetWindowSize(m_window, &result.x, &result.y);
	return result;
}

void SDL3Backend::set_window_size(const Vec2I& size)
{
	SDL_SetWindowSize(m_window, size.x, size.y);
}

Vec2I SDL3Backend::get_screen_size()
{
	const SDL_DisplayMode* out = SDL_GetCurrentDisplayMode(1);

	if (out) {
		return { out->w, out->h };
	}

	return { 0, 0 };
}

float SDL3Backend::get_window_opacity()
{
	float result = 1.0f;
	SDL_GetWindowOpacity(m_window, &result);
	return result;
}

void SDL3Backend::set_window_opacity(float opacity)
{
	SDL_SetWindowOpacity(m_window, opacity);
}

bool SDL3Backend::is_window_resizable()
{
	return SDL_GetWindowFlags(m_window) & SDL_WINDOW_RESIZABLE;
}

void SDL3Backend::toggle_window_resizable(bool toggle)
{
	SDL_SetWindowResizable(m_window, static_cast<SDL_bool>(toggle));
}

float SDL3Backend::get_window_refresh_rate()
{
	const SDL_DisplayMode* out = SDL_GetCurrentDisplayMode(1);

	if (out) {
		return out->refresh_rate;
	}

	return 0.f;
}

float SDL3Backend::get_window_pixel_density()
{
	const SDL_DisplayMode* out = SDL_GetCurrentDisplayMode(1);

	if (out) {
		return out->pixel_density;
	}

	return 0.f;
}

bool SDL3Backend::is_cursor_visible()
{
	return SDL_CursorVisible();
}

void SDL3Backend::toggle_cursor_visible(bool toggle)
{
	if (toggle) {
		SDL_ShowCursor();
	} else {
		SDL_HideCursor();
	}
}

void SDL3Backend::lock_cursor(bool toggle)
{
	SDL_SetRelativeMouseMode(toggle ? SDL_TRUE : SDL_FALSE);
}

void SDL3Backend::set_cursor_position(int x, int y)
{
	SDL_WarpMouseInWindow(m_window, x, y);

	float spx = 0.f, spy = 0.f;
	SDL_GetGlobalMouseState(&spx, &spy);
	inp::Input::get_singleton()->on_mouse_screen_move(spx, spy);
	inp::Input::get_singleton()->on_mouse_move(x, y);
}

WindowMode SDL3Backend::get_window_mode()
{
	auto flags = SDL_GetWindowFlags(m_window);

	if (flags & SDL_WINDOW_FULLSCREEN) {
		return WINDOW_MODE_FULLSCREEN;
	} else if (flags & SDL_WINDOW_BORDERLESS) {
		return WINDOW_MODE_BORDERLESS;
	}

	return WINDOW_MODE_WINDOWED;
}

void SDL3Backend::set_window_mode(WindowMode toggle)
{
	switch (toggle)
	{
	case WINDOW_MODE_FULLSCREEN:
		SDL_SetWindowFullscreen(m_window, SDL_TRUE);
		SDL_SetWindowBordered(m_window, SDL_FALSE);
		break;

	case WINDOW_MODE_BORDERLESS_FULLSCREEN:
		SDL_SetWindowFullscreen(m_window, SDL_TRUE);
		SDL_SetWindowBordered(m_window, SDL_TRUE);
		break;

	case WINDOW_MODE_BORDERLESS:
		SDL_SetWindowFullscreen(m_window, SDL_FALSE);
		SDL_SetWindowBordered(m_window, SDL_FALSE);
		break;

	case WINDOW_MODE_WINDOWED:
		SDL_SetWindowFullscreen(m_window, SDL_FALSE);
		SDL_SetWindowBordered(m_window, SDL_TRUE);

	default:
		return;
	}
}

void SDL3Backend::sleep_for(u64 ms)
{
	if (ms > 0) {
		SDL_Delay(ms);
	}
}

u64 SDL3Backend::ticks()
{
	return SDL_GetTicks();
}

void* SDL3Backend::stream_from_file(const char* filepath, const char* mode)
{
	return SDL_RWFromFile(filepath, mode);
}

void* SDL3Backend::stream_from_memory(void* memory, u64 size)
{
	return SDL_RWFromMem(memory, size);
}

void* SDL3Backend::stream_from_const_memory(const void* memory, u64 size)
{
	return SDL_RWFromConstMem(memory, size);
}

s64 SDL3Backend::stream_read(void* stream, void* ptr, u64 size)
{
	return SDL_RWread((SDL_RWops*)stream, ptr, size);
}

s64 SDL3Backend::stream_write(void* stream, const void* ptr, u64 size)
{
	return SDL_RWwrite((SDL_RWops*)stream, ptr, size);
}

s64 SDL3Backend::stream_seek(void* stream, s64 offset)
{
	return SDL_RWseek((SDL_RWops*)stream, offset, SDL_RW_SEEK_SET);
}

s64 SDL3Backend::stream_size(void* stream)
{
	return SDL_RWsize((SDL_RWops*)stream);
}

s64 SDL3Backend::stream_position(void* stream)
{
	return SDL_RWtell((SDL_RWops*)stream);
}

void SDL3Backend::stream_close(void* stream)
{
	SDL_RWclose((SDL_RWops*)stream);
}

#if wvn_USE_VULKAN

const char* const* SDL3Backend::vk_get_instance_extensions(u32* count)
{
	return SDL_Vulkan_GetInstanceExtensions(count);
}

bool SDL3Backend::vk_create_surface(VkInstance instance, VkSurfaceKHR* surface)
{
	dev::LogMgr::get_singleton()->print("[SDL3] Created Vulkan surface!");
	return SDL_Vulkan_CreateSurface(m_window, instance, NULL, surface);
}

#endif // wvn_USE_VULKAN

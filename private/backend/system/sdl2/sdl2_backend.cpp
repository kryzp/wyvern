#include <backend/system/sdl2/sdl2_backend.h>

#include <wvn/root.h>

#if WVN_USE_VULKAN
#include <SDL_vulkan.h>
#endif

using namespace wvn;
using namespace wvn::sys;

SDL2Backend::SDL2Backend()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0) {
		WVN_ERROR("[SDL2] Failed to initialize")
		//log::error("failed to initialize sdl2: %s", SDL_GetError());
	}

	u64 flags = SDL_WINDOW_ALLOW_HIGHDPI;

	const auto& cfg = Root::get_singleton().config();

	if (cfg.has_flag(Config::FLAG_RESIZABLE)) {
		flags |= SDL_WINDOW_RESIZABLE;
	}

#ifdef WVN_USE_VULKAN
	flags |= SDL_WINDOW_VULKAN;
#endif

	m_window = SDL_CreateWindow(cfg.name, 0, 0, cfg.width, cfg.height, flags);

	if (!m_window) {
		WVN_ERROR("Failed to create window");
	}

	SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	toggle_cursor_visible(cfg.has_flag(Config::FLAG_CURSOR_VISIBLE));

	// todo: move all the window initialization stuff away from the system backend and into root or another manager or something
}

SDL2Backend::~SDL2Backend()
{
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

SystemProperties SDL2Backend::properties()
{
	SystemProperties properties = {};
	properties.temp = false;

	return properties;
}

void SDL2Backend::poll_events()
{
	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
			case SDL_QUIT:
				Root::get_singleton().exit();
				break;

			case SDL_MOUSEWHEEL:
				//Input::inst()->on_mouse_wheel(e.wheel.x, e.wheel.y);
				break;

			case SDL_MOUSEMOTION:
				int spx, spy;
				SDL_GetGlobalMouseState(&spx, &spy);
				//Input::inst()->on_mouse_screen_move(spx, spy);
				//Input::inst()->on_mouse_move(e.motion.x, e.motion.y);
				break;

			case SDL_MOUSEBUTTONDOWN:
				//Input::inst()->on_mouse_down((MouseButton)e.button.button);
				break;

			case SDL_MOUSEBUTTONUP:
				//Input::inst()->on_mouse_up((MouseButton)e.button.button);
				break;

			case SDL_KEYDOWN:
				//Input::inst()->on_key_down((Key)e.key.keysym.scancode);
				break;

			case SDL_KEYUP:
				//Input::inst()->on_key_up((Key)e.key.keysym.scancode);
				break;

			case SDL_TEXTINPUT:
				//Input::inst()->on_text_utf8(e.text.text);
				break;

			case SDL_JOYBUTTONDOWN:
				//Input::inst()->on_joystick_button_down((JoystickButton)e.jbutton.button);
				break;

			case SDL_JOYBUTTONUP:
				//Input::inst()->on_joystick_button_up((JoystickButton)e.jbutton.button);
				break;

			case SDL_JOYBALLMOTION:
				//Input::inst()->on_joystick_ball(e.jball.ball);
				break;

			case SDL_JOYHATMOTION:
				//Input::inst()->on_joystick_hat(e.jhat.value);
				break;

			case SDL_JOYAXISMOTION:
				//float value;
				//if (e.jaxis.value >= 0) value = static_cast<float>(e.jaxis.value) / 32767.0f;
				//else value = static_cast<float>(e.jaxis.value) / 32768.0f;
				//Input::inst()->on_joystick_motion((e.jaxis.axis == 0) ? JS_AXIS_H : JS_AXIS_V, value);
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
	Vec2I result;
	SDL_GetWindowPosition(m_window, &result.x, &result.y);
	return result;
}

void SDL2Backend::set_window_position(const Vec2I& position)
{
	SDL_SetWindowPosition(m_window, position.x, position.y);
}

Vec2I SDL2Backend::get_window_size()
{
	Vec2I result;
	SDL_GetWindowSize(m_window, &result.w, &result.h);
	return result;
}

void SDL2Backend::set_window_size(const Vec2I& size)
{
	SDL_SetWindowSize(m_window, size.w, size.h);
}

Vec2I SDL2Backend::get_screen_size()
{
	SDL_DisplayMode display_mode = {};
	SDL_GetCurrentDisplayMode(0, &display_mode);
	return { display_mode.w, display_mode.h };
}

void SDL2Backend::toggle_cursor_visible(bool toggle)
{
	SDL_ShowCursor(toggle ? SDL_ENABLE : SDL_DISABLE);
}

bool SDL2Backend::is_cursor_visible()
{
	return SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE;
}

void SDL2Backend::sleep_for(u64 ms)
{
	if (ms > 0)
		SDL_Delay(ms);
}

u64 SDL2Backend::ticks()
{
	return SDL_GetTicks64();
}

void* SDL2Backend::stream_from_file(const char* filepath, const char* mode)
{
	return SDL_RWFromFile(filepath, mode);
}

void* SDL2Backend::stream_from_memory(void* memory, s64 size)
{
	return SDL_RWFromMem(memory, size);
}

void* SDL2Backend::stream_from_const_memory(const void* memory, s64 size)
{
	return SDL_RWFromConstMem(memory, size);
}

s64 SDL2Backend::stream_read(void* stream, void* ptr, s64 size)
{
	return SDL_RWread((SDL_RWops*)stream, ptr, sizeof(char), size);
}

s64 SDL2Backend::stream_write(void* stream, const void* ptr, s64 size)
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

bool SDL2Backend::vk_get_instance_extensions(u32* count, const char** names)
{
#if WVN_USE_VULKAN
	return SDL_Vulkan_GetInstanceExtensions(m_window, count, names);
#endif

	return false;
}

#if 0
    void postinit() override
    {
#ifdef LEV_USE_OPENGL
        if (App::inst()->config().vsync)
            SDL_GL_SetSwapInterval(1);
        else
            SDL_GL_SetSwapInterval(0);
#endif
    }

    void present() override
    {
#ifdef LEV_USE_OPENGL
        SDL_GL_SwapWindow(m_window);
#endif

        SDL_ShowWindow(m_window);
    }

    int draw_width() override
    {
        int result = 0;

#ifdef LEV_USE_OPENGL
        SDL_GL_GetDrawableSize(m_window, &result, nullptr);
#else
        result = window_width();
#endif

        return result;
    }

    int draw_height() override
    {
        int result = 0;

#ifdef LEV_USE_OPENGL
        SDL_GL_GetDrawableSize(m_window, nullptr, &result);
#else
        result = window_height();
#endif

        return result;
    }

    void* gl_context_create() override
    {
#ifdef LEV_USE_OPENGL
        return SDL_GL_CreateContext(m_window);
#endif

        return nullptr;
    }

    void gl_context_make_current(void* context) override
    {
#ifdef LEV_USE_OPENGL
        SDL_GL_MakeCurrent(m_window, context);
#endif
    }

    void gl_context_destroy(void* context) override
    {
#ifdef LEV_USE_OPENGL
        SDL_GL_DeleteContext(context);
#endif
    }

    bool gl_load_glad_loader() override
    {
#if LEV_USE_OPENGL
		return gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
#else
		return false;
#endif
    }
};
#endif

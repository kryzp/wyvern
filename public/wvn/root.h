#pragma once

#include <wvn/util/singleton.h>
#include <wvn/actor/actor_mgr.h>
#include <wvn/actor/event_mgr.h>
#include <wvn/physics/physics_mgr.h>
#include <wvn/input/input_mgr.h>
#include <wvn/graphics/rendering_mgr.h>
#include <wvn/graphics/rendering_backend.h>
#include <wvn/system/system_mgr.h>
#include <wvn/system/system_backend.h>
#include <wvn/plugin/plugin.h>
#include <wvn/math/random.h>

#include <wvn/container/vector.h>

// todo: replace with custom implementation
#include <functional>

namespace wvn
{
	struct Config
	{
		enum ConfigFlag
		{
			FLAG_NONE           = 0 << 0,
			FLAG_RESIZABLE      = 1 << 0,
			FLAG_VSYNC          = 1 << 1,
			FLAG_CURSOR_VISIBLE = 1 << 2
		};

		const char* name = nullptr;
		unsigned width = 1280;
		unsigned height = 720;
		unsigned target_fps = 60;
		unsigned max_updates = 5;
		int flags = FLAG_NONE;

		std::function<void(void)> on_init = nullptr;
		std::function<void(void)> on_exit = nullptr;
		std::function<void(void)> on_destroy = nullptr;
		std::function<void(void)> on_update_pre_actor_tick = nullptr;
		std::function<void(void)> on_update_pre_physics = nullptr;
		std::function<void(void)> on_update_post_physics = nullptr;
		std::function<void(void)> on_render = nullptr;

		// utility
		bool has_flag(ConfigFlag flag) const { return flags & flag; }
	};

	class Root : public Singleton<Root>
	{
		WVN_DEF_SINGLETON(Root)

	public:
		Root(const Config& cfg);
		~Root();

		void run();
		void exit();

		float fps() const;
		bool is_running() const;

		const Config& config();

		void show_cursor(bool toggle);
		bool cursor_visible() const;

		void window_position(int x, int y);

		unsigned window_width();
		unsigned window_height();

		unsigned draw_width();
		unsigned draw_height();

		bknd::SystemBackend* current_system_backend();
		void set_system_backend(bknd::SystemBackend* backend);

		bknd::RenderingBackend* current_rendering_backend();
		void set_rendering_backend(bknd::RenderingBackend* backend);

		void add_plugin(Plugin* plugin);

	private:
		void install_plugins();
		void uninstall_plugins();

		SystemMgr* m_system_mgr;
		PhysicsMgr* m_physics_mgr;
		ActorMgr* m_actor_mgr;
		RenderingMgr* m_rendering_mgr;
		InputMgr* m_input_mgr;
		EventMgr* m_event_mgr;
		Random* m_random;

		bknd::SystemBackend* m_system_bknd;
		bknd::RenderingBackend* m_rendering_bknd;

		Config m_config;
		bool m_running;

		Vector<Plugin*> m_plugins;
	};
}

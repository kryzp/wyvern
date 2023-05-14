#include <wvn/root.h>
#include <wvn/plugin/plugin_loader.h>

#include <wvn/system/system_backend.h>
#include <wvn/graphics/renderer_backend.h>
#include <wvn/audio/audio_backend.h>

#include <wvn/actor/actor_mgr.h>
#include <wvn/actor/scene_mgr.h>
#include <wvn/actor/event_mgr.h>
#include <wvn/audio/audio_mgr.h>
#include <wvn/physics/physics_mgr.h>
#include <wvn/input/input_mgr.h>
#include <wvn/graphics/rendering_mgr.h>
#include <wvn/network/network_mgr.h>
#include <wvn/animation/animation_mgr.h>
#include <wvn/resource/resource_mgr.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/devenv/console.h>
#include <wvn/plugin/plugin.h>

#include <wvn/time.h>

using namespace wvn;

WVN_IMPL_SINGLETON(Root);

Root::Root(const Config& cfg)
	: m_config(cfg)
	, m_running(true)
	, random()
{
	m_log_mgr = new dev::LogMgr();
	m_console = new dev::Console();

	m_plugins = plug::PluginLoader::load_plugins();
	install_plugins();

	m_physics_mgr 	= new phys::PhysicsMgr();
	m_actor_mgr 	= new act ::ActorMgr();
	m_scene_mgr 	= new act ::SceneMgr();
	m_event_mgr 	= new act ::EventMgr();
	m_rendering_mgr = new gfx ::RenderingMgr();
	m_audio_mgr 	= new sfx ::AudioMgr();
	m_input_mgr 	= new inp ::InputMgr();
	m_network_mgr 	= new net ::NetworkMgr();
	m_animation_mgr = new anim::AnimationMgr();
	m_resource_mgr  = new res ::ResourceMgr();

	random.regen_seed(cfg.random_seed == 0 ? std::time(nullptr) : cfg.random_seed);

	// update window
	{
		const auto screen_size = m_system_backend->get_screen_size();

		// set title
		m_system_backend->set_window_name(m_config.name);

		// set position
		if (m_config.has_flag(Config::FLAG_CENTRE_WINDOW)) {
			m_system_backend->set_window_position(Vec2I(
				static_cast<int>(screen_size.x - m_config.width) / 2,
				static_cast<int>(screen_size.y - m_config.height) / 2
			));
		}

		// set size
		m_system_backend->set_window_size(Vec2I(m_config.width, m_config.height));

		// set window mode
		m_system_backend->set_window_mode(m_config.window_mode);
		m_system_backend->set_window_opacity(1.0f);

		// set opacity
		m_system_backend->set_window_opacity(m_config.opacity);

		// set window flags
		m_system_backend->toggle_cursor_visible(m_config.has_flag(Config::FLAG_CURSOR_VISIBLE));
		m_system_backend->toggle_window_resizable(m_config.has_flag(Config::FLAG_RESIZABLE));
	}

	if (m_config.on_init) {
		m_config.on_init();
	}

	dev::LogMgr::get_singleton()->print("[ROOT] Initialized!\n--------------------------------");
}

Root::~Root()
{
	// deletion must happen in the precise reverse order that objects are created!

	dev::LogMgr::get_singleton()->print("--------------------------------");

	if (m_config.on_destroy) {
		m_config.on_destroy();
	}

	delete res ::ResourceMgr ::get_singleton();
	delete anim::AnimationMgr::get_singleton();
	delete net ::NetworkMgr  ::get_singleton();
	delete inp ::InputMgr    ::get_singleton();
	delete sfx ::AudioMgr    ::get_singleton();
	delete gfx ::RenderingMgr::get_singleton();
	delete act ::EventMgr    ::get_singleton();
	delete act ::SceneMgr    ::get_singleton();
	delete act ::ActorMgr    ::get_singleton();
	delete phys::PhysicsMgr  ::get_singleton();

	dev::LogMgr::get_singleton()->print("[ROOT] Destroyed!");

	delete dev::LogMgr ::get_singleton();
	delete dev::Console::get_singleton();

	uninstall_plugins();
}

#include <wvn/devenv/profiler.h>

void Root::run()
{
	while (m_running)
	{
		m_system_backend->poll_events();
		m_input_mgr->update();

		// update
		{
			WVN_PROFILE("root.run.update");

			m_actor_mgr->tick_pre_animation();

			m_animation_mgr->tick();

			m_actor_mgr->tick_post_animation();
			m_event_mgr->dispatch_events();

			m_physics_mgr->simulate();

			time::delta = calc_delta_time();
			time::prev_elapsed = time::elapsed;
			time::elapsed += time::delta;
		}

		// audio
		{
			WVN_PROFILE("root.run.audio");

			m_audio_mgr->tick();
		}

		// render
		{
			WVN_PROFILE("root.run.render");

			m_rendering_mgr->render_scene();
			m_rendering_mgr->swap_buffers();
		}

		printf("\n");
	}
}

void Root::exit()
{
	if (m_config.on_exit) {
		m_config.on_exit();
	}

	m_running = false;
}

float Root::fps() const
{
	return m_config.target_fps;
}

bool Root::is_running() const
{
	return m_running;
}

u64 Root::ticks() const
{
	return m_system_backend->ticks();
}

float Root::calc_delta_time() const
{
	return 1.0f / static_cast<float>(m_config.target_fps);
}

const Config& Root::config()
{
	return m_config;
}

sys::SystemBackend* Root::system_backend()
{
	return m_system_backend;
}

void Root::set_system_backend(sys::SystemBackend* backend)
{
	m_system_backend = backend;
}

gfx::RendererBackend* Root::renderer_backend()
{
	return m_rendering_backend;
}

void Root::set_rendering_backend(gfx::RendererBackend* backend)
{
	m_rendering_backend = backend;
}

sfx::AudioBackend* Root::audio_backend()
{
	return m_audio_backend;
}

void Root::set_audio_backend(sfx::AudioBackend* backend)
{
	m_audio_backend = backend;
}

void Root::add_plugin(plug::Plugin* plugin)
{
	m_plugins.push_back(plugin);
}

void Root::install_plugins()
{
	for (auto it = m_plugins.begin(); it != m_plugins.end(); it++) {
		(*it)->install();
	}
}

void Root::uninstall_plugins()
{
	for (auto it = m_plugins.rbegin(); it != m_plugins.rend(); it++) {
		(*it)->uninstall();
	}
}

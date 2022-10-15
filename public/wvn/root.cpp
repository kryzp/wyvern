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
#include <wvn/devenv/log_mgr.h>
#include <wvn/devenv/console.h>

#include <wvn/plugin/plugin.h>

#include <wvn/math/random.h>

using namespace wvn;

WVN_IMPL_SINGLETON(Root);

Root::Root(const Config& cfg)
	: m_config(cfg)
	, m_running(true)
{
	m_log_mgr = new dev::LogMgr();
	m_console = new dev::Console();

	m_plugins = plug::PluginLoader::load_plugins();
	install_plugins();

	m_physics_mgr 	= new phys::PhysicsMgr();
	m_actor_mgr 	= new act::ActorMgr();
	m_scene_mgr 	= new act::SceneMgr();
	m_event_mgr 	= new act::EventMgr();
	m_rendering_mgr = new gfx::RenderingMgr();
	m_audio_mgr 	= new sfx::AudioMgr();
	m_input_mgr 	= new InputMgr();
	m_network_mgr 	= new net::NetworkMgr();
	m_animation_mgr = new anim::AnimationMgr();

	m_random = new Random(cfg.random_seed == 0 ? std::time(nullptr) : cfg.random_seed);

	// update window
	{
		const auto screen_size = m_system_backend->get_screen_size();

		// set title
		m_system_backend->set_window_name(m_config.name);

		// set position
		if (m_config.has_flag(Config::FLAG_CENTRE_WINDOW)) {
			m_system_backend->set_window_position(Vec2I(
				(static_cast<int>(screen_size.w) - static_cast<int>(m_config.width)) / 2,
				(static_cast<int>(screen_size.h) - static_cast<int>(m_config.height)) / 2
			));
		}

		// set size
		m_system_backend->set_window_size(Vec2I(m_config.width, m_config.height));

		// set window mode
		m_system_backend->set_window_mode(m_config.window_mode);
		m_system_backend->set_window_opacity(1.0f);

		// set window flags
		m_system_backend->toggle_cursor_visible(m_config.has_flag(Config::FLAG_CURSOR_VISIBLE));
		m_system_backend->toggle_window_resizable(m_config.has_flag(Config::FLAG_RESIZABLE));
	}

	if (m_config.on_init)
		m_config.on_init();

	dev::LogMgr::get_singleton().print("[ROOT] Initialized!");
}

Root::~Root()
{
	// deletion must happen in the precise reverse order that objects are created!

	if (m_config.on_destroy)
		m_config.on_destroy();

	delete Random::get_singleton_ptr();

	delete anim::AnimationMgr::get_singleton_ptr();
	delete net::NetworkMgr::get_singleton_ptr();
	delete InputMgr::get_singleton_ptr();
	delete sfx::AudioMgr::get_singleton_ptr();
	delete gfx::RenderingMgr::get_singleton_ptr();
	delete act::EventMgr::get_singleton_ptr();
	delete act::SceneMgr::get_singleton_ptr();
	delete act::ActorMgr::get_singleton_ptr();
	delete phys::PhysicsMgr::get_singleton_ptr();

	uninstall_plugins();

	dev::LogMgr::get_singleton().print("[ROOT] Destroyed!");

	delete dev::LogMgr::get_singleton_ptr();
	delete dev::Console::get_singleton_ptr();
}

void Root::run()
{
	while (m_running)
	{
		// poll events
		m_system_backend->poll_events();

		// update input data to reflect polled events from previous call
		m_input_mgr->update();

		// update
		{
			// update actors and dispatch events
			m_actor_mgr->tick();
			m_event_mgr->dispatch_events();

			// simulate physics
			m_physics_mgr->simulate();

			// update animations
			m_animation_mgr->tick();
		}

		// audio
		{
			// update audio
			m_audio_mgr->tick();
		}

		// render
		{
			// draw scene
			m_rendering_mgr->render_scene();

			// finally swap the buffers
			m_rendering_mgr->swap_buffers();
		}
	}
}

void Root::exit()
{
	if (m_config.on_exit)
		m_config.on_exit();

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

const Config& Root::config()
{
	return m_config;
}

sys::SystemBackend* Root::current_system_backend()
{
	return m_system_backend;
}

void Root::set_system_backend(sys::SystemBackend* backend)
{
	m_system_backend = backend;
}

gfx::RendererBackend* Root::current_renderer_backend()
{
	return m_rendering_backend;
}

void Root::set_rendering_backend(gfx::RendererBackend* backend)
{
	m_rendering_backend = backend;
}

sfx::AudioBackend* Root::current_audio_backend()
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
	for (auto it = m_plugins.begin(); it != m_plugins.end(); it++)
		(*it)->install();
}

void Root::uninstall_plugins()
{
	for (auto it = m_plugins.rbegin(); it != m_plugins.rend(); it++)
		(*it)->uninstall();
}

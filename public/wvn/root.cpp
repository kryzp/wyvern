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
#include <wvn/system/window_mgr.h>
#include <wvn/network/network_mgr.h>

#include <wvn/plugin/plugin.h>

#include <wvn/math/random.h>

using namespace wvn;

WVN_IMPL_SINGLETON(Root);

Root::Root(const Config& cfg)
	: m_config(cfg)
	, m_running(true)
{
	m_plugins = plug::PluginLoader::load_plugins();
	install_plugins();

	m_window_mgr = new sys::WindowMgr();
	m_physics_mgr = new phys::PhysicsMgr();
	m_actor_mgr = new act::ActorMgr();
	m_scene_mgr = new act::SceneMgr();
	m_event_mgr = new act::EventMgr();
	m_rendering_mgr = new gfx::RenderingMgr();
	m_audio_mgr = new sfx::AudioMgr();
	m_input_mgr = new InputMgr();
	m_network_mgr = new net::NetworkMgr();

	m_random = new Random();

	if (m_config.on_init)
		m_config.on_init();
}

Root::~Root()
{
	if (m_config.on_destroy)
		m_config.on_destroy();

	delete Random::get_singleton_ptr();

	delete net::NetworkMgr::get_singleton_ptr();
	delete InputMgr::get_singleton_ptr();
	delete sfx::AudioMgr::get_singleton_ptr();
	delete gfx::RenderingMgr::get_singleton_ptr();
	delete act::EventMgr::get_singleton_ptr();
	delete act::SceneMgr::get_singleton_ptr();
	delete act::ActorMgr::get_singleton_ptr();
	delete phys::PhysicsMgr::get_singleton_ptr();
	delete sys::WindowMgr::get_singleton_ptr();

	uninstall_plugins();
}

void Root::run()
{
	while (m_running)
	{
		// poll input
		InputMgr::get_singleton().poll_input();

		// update
		{
			// update actors and dispatch events
			act::ActorMgr::get_singleton().tick();
			act::EventMgr::get_singleton().dispatch_events();

			// simulate physics
			phys::PhysicsMgr::get_singleton().simulate();
		}

		// audio
		{
			sfx::AudioMgr::get_singleton().tick();
		}

		// render
		{
			gfx::RenderingMgr::get_singleton().render_scene();
			gfx::RenderingMgr::get_singleton().swap_buffers();
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
	return m_platform_backend;
}

void Root::set_system_backend(sys::SystemBackend* backend)
{
	m_platform_backend = backend;
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

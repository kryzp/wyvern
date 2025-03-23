#include <wvn/root.h>

#include <wvn/plugin/plugin.h>
#include <wvn/plugin/plugin_loader.h>

#include <wvn/system/system_backend.h>
#include <wvn/graphics/renderer_backend.h>
#include <wvn/audio/audio_backend.h>

#include <wvn/entity/entity_mgr.h>
#include <wvn/entity/event_mgr.h>
#include <wvn/audio/audio_mgr.h>
#include <wvn/physics/physics_mgr.h>
#include <wvn/input/input.h>
#include <wvn/graphics/rendering_mgr.h>
#include <wvn/graphics/mesh_mgr.h>
#include <wvn/graphics/material_system.h>
#include <wvn/network/network_mgr.h>
#include <wvn/animation/animation_mgr.h>
#include <wvn/resource/resource_mgr.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/devenv/console.h>
#include <wvn/devenv/profiler.h>
#include <wvn/maths/timer.h>

#include <wvn/time.h>

using namespace wvn;

wvn_IMPL_SINGLETON(Root);

Root::Root(const Config& cfg)
	: m_config(cfg)
	, m_running(true)
	, random()
	, main_camera(Camera::CAM_PERSP, cfg.width, cfg.height)
{
#if wvn_DEBUG
	m_log_mgr = new dev::LogMgr();
	m_console = new dev::Console();
#endif // wvn_DEBUG

	time::delta = 1.0 / (double)m_config.target_fps;

	m_plugins = plug::PluginLoader::load_plugins();
	install_plugins();

	m_physics_mgr 	= new phys::PhysicsMgr();
	m_entity_mgr 	= new ent ::EntityMgr();
	m_event_mgr 	= new ent ::EventMgr();
	m_mesh_mgr      = new gfx ::MeshMgr();
	m_material_mgr  = new gfx ::MaterialSystem();
	m_rendering_mgr = new gfx ::RenderingMgr();
	m_audio_mgr 	= new sfx ::AudioMgr();
	m_input_mgr 	= new inp ::Input();
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
				(int)(screen_size.x - m_config.width) / 2,
				(int)(screen_size.y - m_config.height) / 2
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

	m_log_mgr->print("[ROOT] Initialized!\n--------------------------------");
}

Root::~Root()
{
	// deletion must happen in the reverse order that objects are created!

	m_log_mgr->print("--------------------------------");

	if (m_config.on_destroy) {
		m_config.on_destroy();
	}

	delete res ::ResourceMgr   ::get_singleton();
	delete anim::AnimationMgr  ::get_singleton();
	delete net ::NetworkMgr    ::get_singleton();
	delete inp ::Input         ::get_singleton();
	delete sfx ::AudioMgr      ::get_singleton();
	delete gfx ::RenderingMgr  ::get_singleton();
	delete gfx ::MaterialSystem::get_singleton();
	delete gfx ::MeshMgr       ::get_singleton();
	delete ent ::EventMgr      ::get_singleton();
	delete ent ::EntityMgr     ::get_singleton();
	delete phys::PhysicsMgr    ::get_singleton();

	uninstall_plugins();

	m_log_mgr->print("[ROOT] Destroyed!");

#if wvn_DEBUG
	m_log_mgr->print("[ROOT] Destroying debug tools...");
	delete dev::LogMgr ::get_singleton();
	delete dev::Console::get_singleton();
#endif // wvn_DEBUG
}

void Root::run()
{
	Timer delta_timer;
	delta_timer.start();

	double accumulator = 0.0;
	time::delta = 1.0 / (double)m_config.target_fps;

	while (m_running)
	{
		// system stuff and input handling
		m_system_backend->poll_events();
		m_input_mgr->update();

		// update entities
		m_entity_mgr->tick_pre_animation();
		m_animation_mgr->tick();
		m_entity_mgr->tick_post_animation();
		m_event_mgr->dispatch_events();

		// update physics
		double frame_time = delta_timer.reset();
		accumulator += CalcD::min(frame_time, time::delta);

		while (accumulator >= time::delta)
		{
			m_physics_mgr->simulate();

			time::elapsed += time::delta;
			accumulator -= time::delta;
		}

		m_physics_mgr->finalize();

		// more entities
		m_entity_mgr->tick_post_physics_update();

		// audio
		m_audio_mgr->tick();

		// render
		m_rendering_mgr->render_scene_and_swap_buffers();
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

void Root::install_plugins()
{
	for (auto it = m_plugins.begin(); it != m_plugins.end(); it++) {
		(*it)->implement();
	}
}

void Root::uninstall_plugins()
{
	for (auto it = m_plugins.rbegin(); it != m_plugins.rend(); it++) {
		(*it)->remove();
	}
}

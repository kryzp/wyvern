#include <wvn/root.h>
#include <wvn/plugin/plugin_loader.h>

using namespace wvn;

WVN_IMPL_SINGLETON(Root);

Root::Root(const Config& cfg)
	: m_config(cfg)
	, m_running(true)
{
	m_plugins = PluginLoader::load_plugins();
	install_plugins();

	m_system_mgr = new SystemMgr();
	m_physics_mgr = new PhysicsMgr();
	m_actor_mgr = new ActorMgr();
	m_rendering_mgr = new RenderingMgr();
	m_input_mgr = new InputMgr();
	m_event_mgr = new EventMgr();
	m_random = new Random();

	if (m_config.on_init)
		m_config.on_init();
}

Root::~Root()
{
	if (m_config.on_destroy)
		m_config.on_destroy();

	delete Random::get_singleton_ptr();
	delete EventMgr::get_singleton_ptr();
	delete InputMgr::get_singleton_ptr();
	delete RenderingMgr::get_singleton_ptr();
	delete ActorMgr::get_singleton_ptr();
	delete PhysicsMgr::get_singleton_ptr();
	delete SystemMgr::get_singleton_ptr();

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
			if (m_config.on_update_pre_actor_tick)
				m_config.on_update_pre_actor_tick();

			// update actors and dispatch events
			ActorMgr::get_singleton().tick();
			EventMgr::get_singleton().dispatch_events();

			if (m_config.on_update_pre_physics)
				m_config.on_update_pre_physics();

			// simulate physics
			PhysicsMgr::get_singleton().simulate();

			if (m_config.on_update_post_physics)
				m_config.on_update_post_physics();
		}

		// render
		{
			if (m_config.on_render)
				m_config.on_render();

			RenderingMgr::get_singleton().render_scene();
			RenderingMgr::get_singleton().swap_buffers();
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

void Root::show_cursor(bool toggle)
{
	m_system_bknd->show_cursor(toggle);
}

bool Root::cursor_visible() const
{
	return m_system_bknd->cursor_visible();
}

void Root::window_position(int x, int y)
{
	m_system_bknd->set_window_position(x, y);
}

unsigned Root::window_width()
{
	return m_system_bknd->window_width();
}

unsigned Root::window_height()
{
	return m_system_bknd->window_height();
}

unsigned Root::draw_width()
{
	return m_system_bknd->draw_width();
}

unsigned Root::draw_height()
{
	return m_system_bknd->draw_height();
}

bknd::SystemBackend* Root::current_system_backend()
{
	return m_system_bknd;
}

void Root::set_system_backend(bknd::SystemBackend* backend)
{
	m_system_bknd = backend;
}

bknd::RenderingBackend* Root::current_rendering_backend()
{
	return m_rendering_bknd;
}

void Root::set_rendering_backend(bknd::RenderingBackend* backend)
{
	m_rendering_bknd = backend;
}

void Root::add_plugin(Plugin* plugin)
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

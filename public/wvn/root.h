#pragma once

#include <wvn/container/vector.h>
#include <wvn/util/singleton.h>

// todo: replace with custom implementation
#include <functional>

namespace wvn
{
	namespace act  { class ActorMgr; class SceneMgr; class EventMgr; }
	namespace phys { class PhysicsMgr; }
	namespace gfx { class RenderingMgr; class RendererBackend; }
	namespace sys { class SystemBackend; }
	namespace sfx { class AudioMgr; class AudioBackend; }
	namespace net { class NetworkMgr; }
	namespace anim { class AnimationMgr; }
	namespace dev { class LogMgr; class Console; }
	namespace plug { class Plugin; }
	class InputMgr;
	class Random;

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
		u64 random_seed = 0;

		std::function<void(void)> on_init = nullptr;
		std::function<void(void)> on_exit = nullptr;
		std::function<void(void)> on_destroy = nullptr;
		std::function<void(const char*)> on_log = nullptr;

		// utility
		constexpr bool has_flag(ConfigFlag flag) const { return flags & flag; }
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

		sys::SystemBackend* current_system_backend();
		void set_system_backend(sys::SystemBackend* backend);

		gfx::RendererBackend* current_renderer_backend();
		void set_rendering_backend(gfx::RendererBackend* backend);

		sfx::AudioBackend* current_audio_backend();
		void set_audio_backend(sfx::AudioBackend* backend);

		void add_plugin(plug::Plugin* plugin);

	private:
		void install_plugins();
		void uninstall_plugins();

		phys::PhysicsMgr* m_physics_mgr;
		act::ActorMgr* m_actor_mgr;
		act::SceneMgr* m_scene_mgr;
		act::EventMgr* m_event_mgr;
		gfx::RenderingMgr* m_rendering_mgr;
		sfx::AudioMgr* m_audio_mgr;
		net::NetworkMgr* m_network_mgr;
		anim::AnimationMgr* m_animation_mgr;
		InputMgr* m_input_mgr;
		dev::LogMgr* m_log_mgr;
		dev::Console* m_console;

		Random* m_random;

		sys::SystemBackend* m_system_backend;
		gfx::RendererBackend* m_rendering_backend;
		sfx::AudioBackend* m_audio_backend;

		Config m_config;
		bool m_running;

		Vector<plug::Plugin*> m_plugins;
	};
}

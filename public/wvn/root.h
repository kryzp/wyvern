#ifndef ROOT_H
#define ROOT_H

#include <wvn/container/vector.h>
#include <wvn/container/function.h>
#include <wvn/util/singleton.h>
#include <wvn/maths/random.h>
#include <wvn/camera.h>

namespace wvn
{
	enum WindowMode
	{
		WINDOW_MODE_WINDOWED,
		WINDOW_MODE_BORDERLESS,
		WINDOW_MODE_FULLSCREEN
	};

	namespace act  { class ActorMgr; class SceneMgr; class EventMgr; }
	namespace phys { class PhysicsMgr; }
	namespace gfx { class RenderingMgr; class RendererBackend; }
	namespace sys { class SystemBackend; }
	namespace sfx { class AudioMgr; class AudioBackend; }
	namespace net { class NetworkMgr; }
	namespace anim { class AnimationMgr; }
	namespace dev { class LogMgr; class Console; }
	namespace plug { class Plugin; }
	namespace res { class ResourceMgr; }
	namespace inp { class InputMgr; }

	struct Config
	{
		enum ConfigFlag
		{
			FLAG_NONE           = 0 << 0,
			FLAG_RESIZABLE      = 1 << 0,
			FLAG_VSYNC          = 1 << 1,
			FLAG_CURSOR_VISIBLE = 1 << 2,
			FLAG_CENTRE_WINDOW  = 1 << 3
		};

		const char* name = nullptr;
		unsigned width = 1280;
		unsigned height = 720;
		unsigned target_fps = 60;
		unsigned max_updates = 5;
		float opacity = 1.0f;
		int flags = FLAG_NONE;
		u64 random_seed = 0;
		WindowMode window_mode = WINDOW_MODE_WINDOWED;

		Function<void(void)> on_init = nullptr;
		Function<void(void)> on_exit = nullptr;
		Function<void(void)> on_destroy = nullptr;
		Function<void(const char*)> on_log = nullptr;

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
		u64 ticks() const;

        const Config& config();

		sys::SystemBackend* system_backend();
		void set_system_backend(sys::SystemBackend* backend);

		gfx::RendererBackend* renderer_backend();
		void set_rendering_backend(gfx::RendererBackend* backend);

		sfx::AudioBackend* audio_backend();
		void set_audio_backend(sfx::AudioBackend* backend);

		void add_plugin(plug::Plugin* plugin);

		Random<> random;
		Camera main_camera;

	private:
		void install_plugins();
		void uninstall_plugins();

		float calc_delta_time();

		phys::PhysicsMgr* m_physics_mgr;
		act::ActorMgr* m_actor_mgr;
		act::SceneMgr* m_scene_mgr;
		act::EventMgr* m_event_mgr;
		gfx::RenderingMgr* m_rendering_mgr;
		sfx::AudioMgr* m_audio_mgr;
		net::NetworkMgr* m_network_mgr;
		anim::AnimationMgr* m_animation_mgr;
		res::ResourceMgr* m_resource_mgr;
		inp::InputMgr* m_input_mgr;
		dev::LogMgr* m_log_mgr;
		dev::Console* m_console;

		sys::SystemBackend* m_system_backend;
		gfx::RendererBackend* m_rendering_backend;
		sfx::AudioBackend* m_audio_backend;

		Config m_config;
		bool m_running;

		Vector<plug::Plugin*> m_plugins;
	};
}

#endif // ROOT_H

#ifndef ROOT_H_
#define ROOT_H_

#include <wvn/container/vector.h>
#include <wvn/container/function.h>
#include <wvn/singleton.h>
#include <wvn/maths/random.h>
#include <wvn/camera.h>

namespace wvn
{
	enum WindowMode
	{
		WINDOW_MODE_NONE,

		WINDOW_MODE_WINDOWED	= 0 << 0,
		WINDOW_MODE_BORDERLESS	= 1 << 0,
		WINDOW_MODE_FULLSCREEN	= 1 << 1,
		WINDOW_MODE_BORDERLESS_FULLSCREEN = WINDOW_MODE_BORDERLESS | WINDOW_MODE_FULLSCREEN,

		WINDOW_MODE_MAX_ENUM
	};

	namespace ent  { class EntityMgr; class EventMgr; }
	namespace phys { class PhysicsMgr; }
	namespace gfx { class RenderingMgr; class RendererBackend; class MeshMgr; class MaterialSystem; }
	namespace sys { class SystemBackend; }
	namespace sfx { class AudioMgr; class AudioBackend; }
	namespace net { class NetworkMgr; }
	namespace anim { class AnimationMgr; }
	namespace dev { class LogMgr; class Console; }
	namespace plug { class Plugin; }
	namespace res { class ResourceMgr; }
	namespace inp { class Input; }

	struct Config
	{
		enum ConfigFlag
		{
			FLAG_NONE,

			FLAG_RESIZABLE      = 1 << 0,
			FLAG_VSYNC          = 1 << 1,
			FLAG_CURSOR_VISIBLE = 1 << 2,
			FLAG_CENTRE_WINDOW  = 1 << 3,

			FLAG_MAX_ENUM
		};

		const char* name = nullptr;
		unsigned width = 1280;
		unsigned height = 720;
		unsigned target_fps = 60;
		unsigned max_updates = 5;
		float opacity = 1.0f;
		int flags = 0;
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
		wvn_DEF_SINGLETON(Root)

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

		Random<> random;
		Camera main_camera;

	private:
		void install_plugins();
		void uninstall_plugins();

		phys::PhysicsMgr* m_physics_mgr;
		ent::EntityMgr* m_entity_mgr;
		ent::EventMgr* m_event_mgr;
		gfx::RenderingMgr* m_rendering_mgr;
		gfx::MeshMgr* m_mesh_mgr;
		gfx::MaterialSystem* m_material_mgr;
		sfx::AudioMgr* m_audio_mgr;
		net::NetworkMgr* m_network_mgr;
		anim::AnimationMgr* m_animation_mgr;
		res::ResourceMgr* m_resource_mgr;
		inp::Input* m_input_mgr;

#if wvn_DEBUG
		dev::LogMgr* m_log_mgr;
		dev::Console* m_console;
#endif // wvn_DEBUG

		sys::SystemBackend* m_system_backend;
		gfx::RendererBackend* m_rendering_backend;
		sfx::AudioBackend* m_audio_backend;

		Config m_config;
		bool m_running;

		Vector<plug::Plugin*> m_plugins;
	};
}

#endif // ROOT_H_

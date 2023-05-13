#include <wvn/root.h>
#include <wvn/input/input_mgr.h>
#include <wvn/actor/actor.h>
#include <wvn/actor/actor_mgr.h>
#include <wvn/actor/event.h>
#include <wvn/actor/event_mgr.h>

class CameraController : public wvn::act::Actor
{
public:
	CameraController()
		: wvn::act::Actor()
	{
	}

	void tick() override
	{
		auto& camera = wvn::Root::get_singleton()->main_camera;

		static float t = 0.0f;

		if (wvn::inp::InputMgr::get_singleton()->is_down(wvn::inp::KEY_Q)) {
			t -= 0.0125f;
		} else if (wvn::inp::InputMgr::get_singleton()->is_down(wvn::inp::KEY_E)) {
			t += 0.0125f;
		}

		wvn::Vec3F direction = wvn::Vec3F::from_angle(0.0f, -t + wvn::CalcF::PI / 2.0f, 1.0f);
		camera.direction = direction;

		if (wvn::inp::InputMgr::get_singleton()->is_down(wvn::inp::KEY_A)) {
			camera.transform.move(-(-wvn::Vec3F::cross(direction, wvn::Vec3F::up()) * 0.025f));
		} else if (wvn::inp::InputMgr::get_singleton()->is_down(wvn::inp::KEY_D)) {
			camera.transform.move(  -wvn::Vec3F::cross(direction, wvn::Vec3F::up()) * 0.025f);
		}

		if (wvn::inp::InputMgr::get_singleton()->is_down(wvn::inp::KEY_SPACE)) {
			camera.transform.move_y(-0.025f); // todo: this should be positive?
		} else if (wvn::inp::InputMgr::get_singleton()->is_down(wvn::inp::KEY_LEFT_SHIFT)) {
			camera.transform.move_y(0.025f);
		}

		if (wvn::inp::InputMgr::get_singleton()->is_down(wvn::inp::KEY_S)) {
			camera.transform.move(-direction * 0.025f);
		} else if (wvn::inp::InputMgr::get_singleton()->is_down(wvn::inp::KEY_W)) {
			camera.transform.move( direction * 0.025f);
		}

		if (wvn::inp::InputMgr::get_singleton()->is_down(wvn::inp::KEY_ESCAPE))
		{
			wvn::act::Event quit_evt("force.quit");
			quit_evt.append_str("data.usage", "test");
			quit_evt.send(this);
		}
	}

	bool on_event(wvn::act::Event& event) override
	{
		if (wvn::act::Actor::on_event(event)) {
			return true;
		}

		if (event.is_type("force.quit")) {
			wvn::Root::get_singleton()->exit();
			return true;
		}

		return false;
	}
};

int main()
{
	wvn::Config cfg;
	{
		cfg.name = "Wyvern Demo";
		cfg.width = 1280;
		cfg.height = 720;
		cfg.target_fps = 144;
		cfg.max_updates = 5;
		cfg.window_mode = wvn::WINDOW_MODE_WINDOWED;
		cfg.flags =
			wvn::Config::FLAG_CURSOR_VISIBLE |
			wvn::Config::FLAG_VSYNC |
			wvn::Config::FLAG_CENTRE_WINDOW;
	}

	new wvn::Root(cfg);
	{
		{
			wvn::Root::get_singleton()->main_camera.fov  = 45.0f;
			wvn::Root::get_singleton()->main_camera.near =  0.1f;
			wvn::Root::get_singleton()->main_camera.far  = 10.0f;
			wvn::act::ActorMgr::get_singleton()->create<CameraController>();
		}

		wvn::Root::get_singleton()->run();
	}
	delete wvn::Root::get_singleton();

	return 0;
}

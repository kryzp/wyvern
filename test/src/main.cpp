#include <wvn/root.h>
#include <wvn/input/input_mgr.h>
#include <wvn/actor/actor.h>
#include <wvn/actor/actor_mgr.h>
#include <wvn/actor/event.h>
#include <wvn/actor/event_mgr.h>

class Object : public wvn::act::Actor
{
public:
	Object()
		: wvn::act::Actor()
	{
	}

	void tick() override
	{
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
		cfg.name = "Wyvern | main.cpp";
		cfg.width = 1280;
		cfg.height = 720;
		cfg.target_fps = 60;
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
			wvn::Root::get_singleton()->main_camera;
			wvn::act::ActorMgr::get_singleton()->create<Object>();
		}

		wvn::Root::get_singleton()->run();
	}
	delete wvn::Root::get_singleton();

	return 0;
}

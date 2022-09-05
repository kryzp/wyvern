#include <wvn/root.h>

#include <wvn/actor/actor.h>
#include <wvn/actor/actor_mgr.h>

#include <iostream>

struct EventType0Pack
{
	float damage;
	char type;
};

namespace evt
{
	EventType0Pack unpack_eventtype0(const wvn::Event& event)
	{
		return {
			.damage = event.args["Damage"].f32,
			.type = event.args["Type"].character
		};
	}
}

class Ball : public wvn::Actor
{
public:
	Ball()
		: wvn::Actor()
	{
	}

	void tick() override
	{
		std::cout << "Hello, World!" << std::endl;

		wvn::Event test_event;
		test_event.type = "EventType0";
		test_event.append_f32("Damage", 3.5f);
		test_event.append_char("Type", 'B');

		test_event.send(*this);

		wvn::Root::get_singleton().exit();
	}

	bool on_event(wvn::Event& event) override
	{
		if (wvn::Actor::on_event(event))
			return true;

		auto pack = evt::unpack_eventtype0(event);

		switch (event.type_hash())
		{
			case SID("EventType0"):
				std::cout << "Damage: " << pack.damage << std::endl;
				std::cout << "thing: " << pack.type << std::endl;
				return true;

			default:
				return false;
		}
	}
};

int main()
{
	wvn::Config cfg;
	{
		cfg.name = "Wyvern Test";
		cfg.width = 1280;
		cfg.height = 720;
		cfg.target_fps = 60;
		cfg.max_updates = 5;
		cfg.flags =
			wvn::Config::FLAG_CURSOR_VISIBLE |
			wvn::Config::FLAG_VSYNC |
			wvn::Config::FLAG_RESIZABLE;
	}

	new wvn::Root(cfg);
	{
		auto ball = wvn::ActorMgr::get_singleton().add<Ball>();
	}
	wvn::Root::get_singleton().run();

	delete wvn::Root::get_singleton_ptr();

	return 0;
}

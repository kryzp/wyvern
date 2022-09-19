#include <wvn/root.h>
#include <wvn/actor/actor.h>
#include <wvn/actor/actor_mgr.h>
#include <wvn/actor/event.h>
#include <wvn/container/string.h>

int main()
{
	wvn::Config cfg;
	{
		cfg.name = "main.cpp";
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
	wvn::Root::get_singleton().run();
	delete wvn::Root::get_singleton_ptr();

	return 0;
}

#include <wvn/container/bitset.h>

int main(int argc, char** argv)
{
#if 0
	wvn::Config cfg;
	{
		cfg.name = "Wyvern | Demo";
		cfg.width = 1280;
		cfg.height = 720;
		cfg.target_fps = 60;
		cfg.max_updates = 5;
		cfg.window_mode = wvn::WINDOW_MODE_WINDOWED;
		cfg.flags =
			wvn::Config::FLAG_CURSOR_VISIBLE |
			wvn::Config::FLAG_VSYNC |
			wvn::Config::FLAG_RESIZABLE |
			wvn::Config::FLAG_CENTRE_WINDOW;
	}

	new wvn::Root(cfg);
	wvn::Root::get_singleton().run();
	delete wvn::Root::get_singleton_ptr();
#endif

	return 0;
}

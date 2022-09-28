#include <wvn/math/complex.h>

#include <iostream>

void print_complex(const wvn::Complex& c)
{
	std::cout << "Re: " << c.real << ", Im: " << c.imag << std::endl;
}

int main()
{
	print_complex(wvn::Complex::fact(wvn::Complex(4.0, 0.0)));

//	wvn::Config cfg;
//	{
//		cfg.name = "main.cpp";
//		cfg.width = 1280;
//		cfg.height = 720;
//		cfg.target_fps = 60;
//		cfg.max_updates = 5;
//		cfg.flags =
//			wvn::Config::FLAG_CURSOR_VISIBLE |
//			wvn::Config::FLAG_VSYNC |
//			wvn::Config::FLAG_RESIZABLE;
//	}

//	new wvn::Root(cfg);
//	wvn::Root::get_singleton().run();
//	delete wvn::Root::get_singleton_ptr();

	return 0;
}

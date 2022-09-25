#include <wvn/math/complex.h>

#include <iostream>

void print_complex(const wvn::Complex& c)
{
	std::cout << "Re: " << c.real << ", Im: " << c.imag << std::endl;
}

int main()
{
	wvn::Complex c0(25.0, 0.0);
	wvn::Complex c1(2.0, 0.0);

	auto c0sqr = wvn::Complex::root(c0, wvn::Complex(5.0, 0.0));
	auto c1pow = wvn::Complex::pow(c1, wvn::Complex(2.0, 0.0));

	print_complex(c0sqr);
	print_complex(c1pow);

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

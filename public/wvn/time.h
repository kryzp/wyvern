#ifndef TIME_H_
#define TIME_H_

namespace wvn::time
{
	extern double elapsed; // in seconds
	extern double delta; // in seconds

	inline double elapsed_ms() { return elapsed * 1000.0; }
	inline double delta_ms() { return delta * 1000.0; }
}

#endif // TIME_H_

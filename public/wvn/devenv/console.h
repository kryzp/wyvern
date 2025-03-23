#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <wvn/singleton.h>

namespace wvn::dev
{
	/**
	 * Developer Console.
	 */
	class Console : public Singleton<Console>
	{
		wvn_DEF_SINGLETON(Console);

	public:
		Console();
		~Console();
	};
}

#endif // CONSOLE_H_

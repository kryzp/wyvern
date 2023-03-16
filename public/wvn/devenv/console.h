#ifndef CONSOLE_H
#define CONSOLE_H

#include <wvn/util/singleton.h>

namespace wvn::dev
{
	/**
	 * Developer Console.
	 */
	class Console : public Singleton<Console>
	{
		WVN_DEF_SINGLETON(Console);

	public:
		Console();
		~Console();
	};
}

#endif // CONSOLE_H

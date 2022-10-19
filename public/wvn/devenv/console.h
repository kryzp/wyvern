#pragma once

#include <wvn/util/singleton.h>

namespace wvn::dev
{
	/*
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

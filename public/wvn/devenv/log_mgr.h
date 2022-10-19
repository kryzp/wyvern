#pragma once

#include <wvn/util/singleton.h>

namespace wvn::dev
{
	/*
	 * Mamages logging different things, simple things such as
	 * printing to the console or other more sophisticated forms
	 * of logging information for later examination.
	 */
	class LogMgr : public Singleton<LogMgr>
	{
		WVN_DEF_SINGLETON(LogMgr);

	public:
		LogMgr();
		~LogMgr();

		void print(const char* fmt, ...);
	};
}

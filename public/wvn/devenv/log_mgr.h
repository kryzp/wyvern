#pragma once

#include <wvn/util/singleton.h>

namespace wvn::dev
{
	class LogMgr : public Singleton<LogMgr>
	{
		WVN_DEF_SINGLETON(LogMgr);

	public:
		LogMgr();
		~LogMgr();

		void print(const char* fmt, ...);
	};
}

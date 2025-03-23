#include <wvn/devenv/log_mgr.h>
#include <wvn/root.h>

#include <stdarg.h>
#include <stdio.h>

using namespace wvn;
using namespace wvn::dev;

wvn_IMPL_SINGLETON(LogMgr);

LogMgr::LogMgr()
{
}

LogMgr::~LogMgr()
{
}

void LogMgr::print(const char* fmt, ...)
{
	va_list valist;
	va_start(valist, fmt);
	char fmt1[512];
	vsprintf(fmt1, fmt, valist);
	printf("%s\n", fmt1);
	va_end(valist);

	if (Root::get_singleton()->config().on_log) {
		Root::get_singleton()->config().on_log(fmt1);
	}
}

#pragma once

#include <wvn/util/singleton.h>

namespace wvn
{
	class SystemMgr : public Singleton<SystemMgr>
	{
		WVN_DEF_SINGLETON(SystemMgr)

	public:
		SystemMgr();
		~SystemMgr();
	};
}

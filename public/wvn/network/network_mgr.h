#pragma once

#include <wvn/util/singleton.h>

namespace wvn::net
{
	class NetworkMgr : public Singleton<NetworkMgr>
	{
		WVN_DEF_SINGLETON(NetworkMgr);

	public:
		NetworkMgr();
		~NetworkMgr();
	};
}

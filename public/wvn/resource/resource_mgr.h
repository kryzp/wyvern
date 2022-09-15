#pragma once

#include <wvn/util/singleton.h>

namespace wvn::res
{
	class ResourceMgr : public Singleton<ResourceMgr>
	{
		WVN_DEF_SINGLETON(ResourceMgr);

	public:
		ResourceMgr();
		~ResourceMgr();
	};
}

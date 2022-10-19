#pragma once

#include <wvn/util/singleton.h>

namespace wvn::res
{
	/*
	 * Manages different resources that can be loaded in-and-out
	 * as the program runs.
	 */
	class ResourceMgr : public Singleton<ResourceMgr>
	{
		WVN_DEF_SINGLETON(ResourceMgr);

	public:
		ResourceMgr();
		~ResourceMgr();
	};
}

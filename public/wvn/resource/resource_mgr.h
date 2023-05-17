#ifndef RESOURCE_MGR_H
#define RESOURCE_MGR_H

#include <wvn/singleton.h>

namespace wvn::res
{
	/**
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

#endif // RESOURCE_MGR_H

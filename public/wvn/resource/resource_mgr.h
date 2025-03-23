#ifndef RESOURCE_MGR_H_
#define RESOURCE_MGR_H_

#include <wvn/singleton.h>

namespace wvn::res
{
	/**
	 * Manages different resources that can be loaded in-and-out
	 * as the program runs.
	 */
	class ResourceMgr : public Singleton<ResourceMgr>
	{
		wvn_DEF_SINGLETON(ResourceMgr);

	public:
		ResourceMgr();
		~ResourceMgr();
	};
}

#endif // RESOURCE_MGR_H_

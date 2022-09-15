#pragma once

#include <wvn/util/singleton.h>

namespace wvn::gfx
{
	class MeshMgr : public Singleton<MeshMgr>
	{
		WVN_DEF_SINGLETON(MeshMgr);

	public:
		MeshMgr();
		~MeshMgr();
	};
}
#include <wvn/resource/resource_mgr.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::res;

WVN_IMPL_SINGLETON(ResourceMgr);

ResourceMgr::ResourceMgr()
{
	dev::LogMgr::get_singleton()->print("[RESOURCE] Initialized!");
}

ResourceMgr::~ResourceMgr()
{
	dev::LogMgr::get_singleton()->print("[RESOURCE] Destroyed!");
}

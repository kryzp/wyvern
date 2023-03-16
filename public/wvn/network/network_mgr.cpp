#include <wvn/network/network_mgr.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::net;

WVN_IMPL_SINGLETON(NetworkMgr);

NetworkMgr::NetworkMgr()
{
	dev::LogMgr::get_singleton()->print("[NETWORK] Initialized!");
}

NetworkMgr::~NetworkMgr()
{
	dev::LogMgr::get_singleton()->print("[NETWORK] Destroyed!");
}

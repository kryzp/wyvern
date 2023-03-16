#include <wvn/actor/scene_mgr.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::act;

WVN_IMPL_SINGLETON(SceneMgr);

SceneMgr::SceneMgr()
{
	dev::LogMgr::get_singleton()->print("[SCENE] Initialized!");
}

SceneMgr::~SceneMgr()
{
	dev::LogMgr::get_singleton()->print("[SCENE] Destroyed!");
}

#include <wvn/animation/animation_mgr.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::anim;

WVN_IMPL_SINGLETON(AnimationMgr);

AnimationMgr::AnimationMgr()
{
	dev::LogMgr::get_singleton().print("[ANIMATION] Initialized!");
}

AnimationMgr::~AnimationMgr()
{
	dev::LogMgr::get_singleton().print("[ANIMATION] Destroyed!");
}

void AnimationMgr::tick()
{
}

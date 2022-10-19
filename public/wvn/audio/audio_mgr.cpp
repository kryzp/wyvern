#include <wvn/audio/audio_mgr.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::sfx;

WVN_IMPL_SINGLETON(AudioMgr);

AudioMgr::AudioMgr()
{
	dev::LogMgr::get_singleton().print("[AUDIO] Initialized!");
}

AudioMgr::~AudioMgr()
{
	dev::LogMgr::get_singleton().print("[AUDIO] Destroyed!");
}

void AudioMgr::tick()
{
}

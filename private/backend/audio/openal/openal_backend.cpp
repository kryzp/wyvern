#include <backend/audio/openal/openal_backend.h>

#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::sfx;

OpenALBackend::OpenALBackend()
{
	dev::LogMgr::get_singleton().print("[OPENAL] Initialized!");
}

OpenALBackend::~OpenALBackend()
{
	dev::LogMgr::get_singleton().print("[OPENAL] Destroyed!");
}

AudioBackendProperties OpenALBackend::properties()
{
	AudioBackendProperties properties = {};
	properties.temp = false;

	return properties;
}

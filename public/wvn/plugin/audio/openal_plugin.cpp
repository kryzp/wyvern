#include <wvn/plugin/audio/openal_plugin.h>
#include <backend/audio/openal_backend.h>
#include <wvn/root.h>

using namespace wvn;
using namespace wvn::plug;

void OpenALPlugin::install()
{
	m_backend = new sfx::OpenALBackend();
	Root::get_singleton().set_audio_backend((sfx::OpenALBackend*)m_backend);
}

void OpenALPlugin::uninstall()
{
	delete (sfx::OpenALBackend*)m_backend;
}

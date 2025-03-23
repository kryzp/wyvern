#include <wvn/plugin/audio/openal_plugin.h>
#include <backend/audio/openal/openal_backend.h>
#include <wvn/root.h>

using namespace wvn;
using namespace wvn::plug;

void OpenALPlugin::implement()
{
	m_backend = new sfx::OpenALBackend();
	Root::get_singleton()->set_audio_backend(static_cast<sfx::OpenALBackend*>(m_backend));
}

void OpenALPlugin::remove()
{
	delete static_cast<sfx::OpenALBackend*>(m_backend);
}

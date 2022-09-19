#include <backend/audio/openal/openal_backend.h>

using namespace wvn;
using namespace wvn::sfx;

OpenALBackend::OpenALBackend()
{
}

OpenALBackend::~OpenALBackend()
{
}

AudioProperties OpenALBackend::properties()
{
	AudioProperties properties = {};
	properties.temp = false;

	return properties;
}

#ifndef OPENAL_BACKEND_H
#define OPENAL_BACKEND_H

#include <wvn/audio/audio_backend.h>

namespace wvn::sfx
{
	class OpenALBackend : public AudioBackend
	{
	public:
		OpenALBackend();
		~OpenALBackend() override;

		AudioBackendProperties properties() override;
	};
}

#endif // OPENAL_BACKEND_H

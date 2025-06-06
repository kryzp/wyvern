#ifndef OPENAL_BACKEND_H_
#define OPENAL_BACKEND_H_

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

#endif // OPENAL_BACKEND_H_

#pragma once

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

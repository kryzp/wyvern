#ifndef AUDIO_BACKEND_H
#define AUDIO_BACKEND_H

namespace wvn::sfx
{
	/**
	 * Properties that an audio backend might have that are exclusive to itself.
	 */
	struct AudioBackendProperties
	{
		bool temp;
	};

	/**
	 * Audio backend that can be implemented by specific API's to
	 * disconnect them from being directly connected to the actual game
	 * engine, effectively acting as an interface.
	 */
	class AudioBackend
	{
	public:
		AudioBackend() = default;
		virtual ~AudioBackend() = default;

		virtual AudioBackendProperties properties() = 0;
	};
}

#endif // AUDIO_BACKEND_H

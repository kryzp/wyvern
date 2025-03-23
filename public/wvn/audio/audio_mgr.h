#ifndef AUDIO_MGR_H_
#define AUDIO_MGR_H_

#include <wvn/singleton.h>

namespace wvn::sfx
{
	/**
	 * Manages the audio in the game.
	 */
	class AudioMgr : public Singleton<AudioMgr>
	{
		wvn_DEF_SINGLETON(AudioMgr);

	public:
		AudioMgr();
		~AudioMgr();

		void tick();
	};
}

#endif // AUDIO_MGR_H_

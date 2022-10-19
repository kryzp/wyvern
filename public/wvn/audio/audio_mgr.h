#pragma once

#include <wvn/util/singleton.h>

namespace wvn::sfx
{
	/*
	 * Manages the audio in the game.
	 */
	class AudioMgr : public Singleton<AudioMgr>
	{
		WVN_DEF_SINGLETON(AudioMgr);

	public:
		AudioMgr();
		~AudioMgr();

		void tick();
	};
}

#pragma once

#include <wvn/util/singleton.h>

namespace wvn::anim
{
	class AnimationMgr : public Singleton<AnimationMgr>
	{
		WVN_DEF_SINGLETON(AnimationMgr);

	public:
		AnimationMgr();
		~AnimationMgr();

		void tick();
	};
}

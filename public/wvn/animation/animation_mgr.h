#ifndef ANIMATION_MGR_H_
#define ANIMATION_MGR_H_

#include <wvn/singleton.h>

namespace wvn::anim
{
	/**
	 * Manages animations in models and other things that happen
	 * over time and need to be interpolated in some way.
	 */
	class AnimationMgr : public Singleton<AnimationMgr>
	{
		wvn_DEF_SINGLETON(AnimationMgr);

	public:
		AnimationMgr();
		~AnimationMgr();

		void tick();
	};
}

#endif // ANIMATION_MGR_H_

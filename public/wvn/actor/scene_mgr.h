#ifndef SCENE_MGR_H
#define SCENE_MGR_H

#include <wvn/util/singleton.h>

namespace wvn::act
{
	/**
	 * Represents a scene in the game world.
	 */
	class Scene
	{
	};

	/**
	 * Manages the current scene in the game world.
	 */
	class SceneMgr : public Singleton<SceneMgr>
	{
		WVN_DEF_SINGLETON(SceneMgr);

	public:
		SceneMgr();
		~SceneMgr();

		void load();
		void unload();
	};
}

#endif // SCENE_MGR_H

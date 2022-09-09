#pragma once

#include <wvn/util/singleton.h>

namespace wvn::act
{
	class Scene
	{
	};

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

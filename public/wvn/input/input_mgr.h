#pragma once

#include <wvn/util/singleton.h>

namespace wvn
{
	class InputMgr : public Singleton<InputMgr>
	{
		WVN_DEF_SINGLETON(InputMgr)

	public:
		InputMgr();
		~InputMgr();

		void update();
	};
}

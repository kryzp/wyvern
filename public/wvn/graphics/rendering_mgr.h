#pragma once

#include <wvn/util/singleton.h>

namespace wvn
{
	class RenderingMgr : public Singleton<RenderingMgr>
	{
		WVN_DEF_SINGLETON(RenderingMgr);

	public:
		RenderingMgr();
		~RenderingMgr();

		void render_scene();
		void swap_buffers();
	};
}

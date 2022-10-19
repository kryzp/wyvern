#pragma once

#include <wvn/util/singleton.h>

namespace wvn::gfx
{
	/*
	 * Responsible for the management of textures.
	 */
	class TextureMgr : public Singleton<TextureMgr>
	{
		WVN_DEF_SINGLETON(TextureMgr);

	public:
		TextureMgr();
		~TextureMgr();
	};
}
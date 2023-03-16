#ifndef TEXTURE_MGR_H
#define TEXTURE_MGR_H

#include <wvn/util/singleton.h>
#include <wvn/util/types.h>
#include <wvn/graphics/texture.h>
#include <wvn/graphics/image.h>

namespace wvn::gfx
{
	/**
	 * Responsible for the management of textures.
	 */
	class TextureMgr : public Singleton<TextureMgr>
	{
		WVN_DEF_SINGLETON(TextureMgr);

	public:
		TextureMgr();
		~TextureMgr();

		Ref<Texture> create(const Image& image);
	};
}
#endif // TEXTURE_MGR_H

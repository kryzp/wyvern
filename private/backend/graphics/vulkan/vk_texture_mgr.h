#ifndef VK_TEXTURE_MGR_H
#define VK_TEXTURE_MGR_H

#include <wvn/graphics/texture_mgr.h>

namespace wvn::gfx
{
	class VulkanTextureMgr : public TextureMgr
	{
	public:
		VulkanTextureMgr();
		~VulkanTextureMgr() override;
	};
}

#endif // VK_TEXTURE_MGR_H

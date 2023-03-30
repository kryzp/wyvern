#ifndef VK_TEXTURE_MGR_H
#define VK_TEXTURE_MGR_H

#include <wvn/container/vector.h>
#include <wvn/graphics/texture_mgr.h>

namespace wvn::gfx
{
	class VulkanBackend;
	class VulkanTexture;

	class VulkanTextureMgr : public TextureMgr
	{
	public:
		VulkanTextureMgr(VulkanBackend* backend);
		~VulkanTextureMgr() override;

		Texture* create(const Image& image) override;
		Texture* create(u32 width, u32 height, TextureFormat format, TextureTiling tiling, const byte* data, u64 size) override;

	private:
		VulkanBackend* m_backend;
		Vector<VulkanTexture*> m_textures;
	};
}

#endif // VK_TEXTURE_MGR_H

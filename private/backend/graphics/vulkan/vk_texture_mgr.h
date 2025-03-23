#ifndef VK_TEXTURE_MGR_H_
#define VK_TEXTURE_MGR_H_

#include <wvn/container/vector.h>
#include <wvn/graphics/texture_mgr.h>

namespace wvn::gfx
{
	class VulkanBackend;
	class VulkanTexture;
	class VulkanTextureSampler;

	class VulkanTextureMgr : public TextureMgr
	{
	public:
		VulkanTextureMgr(VulkanBackend* backend);
		~VulkanTextureMgr() override;

		Texture* create(const Image& image) override;
		Texture* create(u32 width, u32 height, TextureFormat format, TextureTiling tiling, const byte* data, u64 size) override;
		Texture* create_attachment(u32 width, u32 height, TextureFormat format, TextureTiling tiling) override;
		Texture* create_cube_map(TextureFormat format, const Image& right, const Image& left, const Image& top, const Image& bottom, const Image& front, const Image& back) override;

		TextureSampler* create_sampler(const TextureSampler::Style& style) override;

	private:
		VulkanBackend* m_backend;
	};
}

#endif // VK_TEXTURE_MGR_H_

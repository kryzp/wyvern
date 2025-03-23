#ifndef VK_RENDER_TARGET_MGR_H_
#define VK_RENDER_TARGET_MGR_H_

#include <wvn/container/vector.h>
#include <wvn/graphics/render_target_mgr.h>

namespace wvn::gfx
{
    class VulkanBackend;
    class VulkanRenderTarget;
	class VulkanTexture;

    class VulkanRenderTargetMgr : public RenderTargetMgr
    {
    public:
        VulkanRenderTargetMgr(VulkanBackend* backend);
        ~VulkanRenderTargetMgr() override;

		RenderTarget* create_target(u32 width, u32 height) override;
		RenderTarget* create_depth_target(u32 width, u32 height) override;

    private:
        VulkanBackend* m_backend;
    };
}

#endif // VK_RENDER_TARGET_MGR_H_

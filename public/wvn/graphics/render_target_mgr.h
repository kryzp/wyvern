#ifndef RENDER_TARGET_MGR_H_
#define RENDER_TARGET_MGR_H_

#include <wvn/singleton.h>
#include <wvn/common.h>
#include <wvn/container/vector.h>
#include <wvn/graphics/render_target.h>

namespace wvn::gfx
{
    class RenderTargetMgr : public Singleton<RenderTargetMgr>
    {
        wvn_DEF_SINGLETON(RenderTargetMgr);

    public:
        RenderTargetMgr();
        virtual ~RenderTargetMgr();

		RenderTarget* get_target(u32 width, u32 height);
		RenderTarget* get_depth_only_target(u32 width, u32 height);

	protected:
		virtual RenderTarget* create_target(u32 width, u32 height) = 0;
		virtual RenderTarget* create_depth_target(u32 width, u32 height) = 0;

	private:
		Vector<RenderTarget*> m_targets;
    };
}

#endif // RENDER_TARGET_MGR_H_

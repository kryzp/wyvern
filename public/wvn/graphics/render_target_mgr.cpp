#include <wvn/graphics/render_target_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

wvn_IMPL_SINGLETON(RenderTargetMgr);

RenderTargetMgr::RenderTargetMgr()
	: m_targets()
{
}

RenderTargetMgr::~RenderTargetMgr()
{
	for (auto& t : m_targets)
		delete t;

	m_targets.clear();
}

RenderTarget* RenderTargetMgr::get_target(u32 width, u32 height)
{
	RenderTarget* target = create_target(width, height);
	m_targets.push_back(target);
	return target;
}

RenderTarget* RenderTargetMgr::get_depth_only_target(u32 width, u32 height)
{
	RenderTarget* target = create_depth_target(width, height);
	m_targets.push_back(target);
	return target;
}

#include <wvn/graphics/rendering_mgr.h>
#include <wvn/graphics/renderer_backend.h>
#include <wvn/root.h>

using namespace wvn;
using namespace wvn::gfx;

WVN_IMPL_SINGLETON(RenderingMgr);

RenderingMgr::RenderingMgr()
{
}

RenderingMgr::~RenderingMgr()
{
}

void RenderingMgr::render_scene()
{
	auto* renderer = Root::get_singleton().current_renderer_backend();

	renderer->wait_for_sync();
	renderer->clear(Colour::black());
	renderer->debug_render();
}

void RenderingMgr::swap_buffers()
{
}

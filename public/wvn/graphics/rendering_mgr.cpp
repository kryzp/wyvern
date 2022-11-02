#include <wvn/graphics/rendering_mgr.h>
#include <wvn/graphics/renderer_backend.h>
#include <wvn/root.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

WVN_IMPL_SINGLETON(RenderingMgr);

RenderingMgr::RenderingMgr()
{
	dev::LogMgr::get_singleton().print("[RENDERING] Initialized!");
}

RenderingMgr::~RenderingMgr()
{
	dev::LogMgr::get_singleton().print("[RENDERING] Destroyed!");
}

void RenderingMgr::render_scene()
{
	auto* renderer = Root::get_singleton().current_renderer_backend();

	renderer->wait_for_sync();

	static float time = 0.0f;
	time += 1.0f / 500.0f;

	RenderPass pass = {
		.clear_colour = Colour(
			(CalcF::cos(time * 4.0f) + 1.0f) * 0.5f * 255.0f,
			(CalcF::sin(time * CalcF::PI) + 1.0f) * 0.5f * 255.0f,
			(CalcF::sin(time * 2.0f + 3.0f) + 1.0f) * 0.5f * 255.0f,
			255
		)
	};

	renderer->render(pass);
}

void RenderingMgr::swap_buffers()
{
}

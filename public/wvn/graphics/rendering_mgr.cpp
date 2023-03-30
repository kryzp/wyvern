#include <wvn/graphics/rendering_mgr.h>
#include <wvn/graphics/renderer_backend.h>
#include <wvn/graphics/texture_mgr.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/root.h>

using namespace wvn;
using namespace wvn::gfx;

WVN_IMPL_SINGLETON(RenderingMgr);

RenderingMgr::RenderingMgr()
{
	Vector<Vertex> box_vertices = {
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
		{ {  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } },
		{ { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
		{ {  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } },
		{ { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 1.0f } },
	};

	Vector<u16> box_indices = {
		0, 1, 2,
		2, 3, 0,

		5, 4, 7,
		7, 6, 5,

		1, 5, 6,
		6, 2, 1,

		4, 0, 3,
		3, 7, 4,

		4, 5, 1,
		1, 0, 4,

		3, 2, 6,
		6, 7, 3
	};

	m = new Mesh(box_vertices, box_indices);
	t = TextureMgr::get_singleton()->create("../images/kitty.png");

	dev::LogMgr::get_singleton()->print("[RENDERING] Initialized!");
}

RenderingMgr::~RenderingMgr()
{
	dev::LogMgr::get_singleton()->print("[RENDERING] Destroyed!");
}

void RenderingMgr::render_scene()
{
	auto* renderer = Root::get_singleton()->renderer_backend();

	renderer->set_texture(0, t);

	renderer->render({
		.clear_colour = Colour::black(),
		.mesh = m
	});
}

void RenderingMgr::swap_buffers()
{
	Root::get_singleton()->renderer_backend()->swap_buffers();
}

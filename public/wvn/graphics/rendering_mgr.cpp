#include <wvn/graphics/rendering_mgr.h>
#include <wvn/graphics/renderer_backend.h>
#include <wvn/graphics/texture_mgr.h>
#include <wvn/graphics/shader_mgr.h>
#include <wvn/input/input_mgr.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/root.h>
#include <wvn/time.h>

using namespace wvn;
using namespace wvn::gfx;

WVN_IMPL_SINGLETON(RenderingMgr);

RenderingMgr::RenderingMgr()
	: m(nullptr)
	, vertex_shader(nullptr)
	, fragment_shader(nullptr) // todo: these should be in the material, just not yet because i don't know how i want to implement this yet
	, material()
{
	Vector<Vertex> box_vertices = {
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
		{ {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
		{ { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
		{ {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
		{ { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
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

	material.texture(TextureMgr::get_singleton()->create("../test/res/kitty.png"));
	material.sampler(TextureMgr::get_singleton()->create_sampler(gfx::TEX_FILTER_LINEAR));

	vertex_shader = ShaderMgr::get_singleton()->create("../test/res/vert.spv", SHADER_TYPE_VERTEX);
	fragment_shader = ShaderMgr::get_singleton()->create("../test/res/frag.spv", SHADER_TYPE_FRAGMENT);

	dev::LogMgr::get_singleton()->print("[RENDERING] Initialized!");
}

RenderingMgr::~RenderingMgr()
{
	dev::LogMgr::get_singleton()->print("[RENDERING] Destroyed!");
}

void RenderingMgr::render_scene()
{
	static float r = 0.0f;
	r += time::delta;

	auto* renderer = Root::get_singleton()->renderer_backend();

	renderer->set_texture(0, material.texture());
	renderer->set_sampler(0, material.sampler());

	renderer->bind_shader(vertex_shader);
	renderer->bind_shader(fragment_shader);

	renderer->render({
		.clear_colour = Colour::black(),
		.mesh = m,
		.model_matrix = Mat4x4::create_rotation(r, Vec3F::up()) * Mat4x4::create_translation(0.0f, 0.0f, 5.0f),
		.view_matrix = Root::get_singleton()->main_camera.view_matrix(), // todo: combine the two matrices into one matrix???
		.proj_matrix = Root::get_singleton()->main_camera.proj_matrix(),
		.target = nullptr
	});
}

void RenderingMgr::swap_buffers()
{
	Root::get_singleton()->renderer_backend()->swap_buffers();
}

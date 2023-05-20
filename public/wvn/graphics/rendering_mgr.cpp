#include <wvn/graphics/rendering_mgr.h>
#include <wvn/graphics/renderer_backend.h>
#include <wvn/graphics/texture_mgr.h>
#include <wvn/graphics/shader_mgr.h>
#include <wvn/graphics/mesh_mgr.h>
#include <wvn/graphics/model.h>
#include <wvn/input/input_mgr.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/actor/actor_mgr.h>
#include <wvn/root.h>
#include <wvn/time.h>

using namespace wvn;
using namespace wvn::gfx;

WVN_IMPL_SINGLETON(RenderingMgr);

RenderingMgr::RenderingMgr()
	: m_vertex_shader(nullptr)
	, m_fragment_shader(nullptr) // todo: these should be in the material, just not yet because i don't know how i want to implement this yet
	, m_models()
	, m_skybox_texture(nullptr)
	, m_skybox_sampler(nullptr)
	, m_skybox_mesh(nullptr)
	, m_skybox_fragment(nullptr)
{
	auto* renderer = Root::get_singleton()->renderer_backend();

	m_vertex_shader = ShaderMgr::get_singleton()->create("../test/res/vert.spv", SHADER_TYPE_VERTEX);
	m_fragment_shader = ShaderMgr::get_singleton()->create("../test/res/frag.spv", SHADER_TYPE_FRAGMENT);
	m_skybox_fragment = ShaderMgr::get_singleton()->create("../test/res/skybox_frag.spv", SHADER_TYPE_FRAGMENT);

	renderer->bind_shader(m_vertex_shader);

	create_skybox();

	dev::LogMgr::get_singleton()->print("[RENDERING] Initialized!");
}

RenderingMgr::~RenderingMgr()
{
	for (auto& m : m_models) {
		delete m;
	}

	dev::LogMgr::get_singleton()->print("[RENDERING] Destroyed!");
}

void RenderingMgr::create_skybox()
{
	Image img_rt("../test/res/skybox/right.jpg");
	Image img_lf("../test/res/skybox/left.jpg");
	Image img_tp("../test/res/skybox/top.jpg");
	Image img_bt("../test/res/skybox/bottom.jpg");
	Image img_ft("../test/res/skybox/front.jpg");
	Image img_bk("../test/res/skybox/back.jpg");

	m_skybox_texture = TextureMgr::get_singleton()->create_cube_map(TEX_FMT_R8G8B8A8_SRGB,
		img_rt, img_lf, img_tp, img_bt, img_ft, img_bk
	);

	m_skybox_sampler = TextureMgr::get_singleton()->create_sampler(TEX_FILTER_LINEAR);

	Vector<Vertex> skybox_vertices = {
		{ { -1.0,  1.0,  1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
		{ { -1.0, -1.0,  1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
		{ {  1.0, -1.0,  1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
		{ {  1.0,  1.0,  1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
		{ { -1.0,  1.0, -1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
		{ { -1.0, -1.0, -1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
		{ {  1.0, -1.0, -1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
		{ {  1.0,  1.0, -1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } }
	};

	Vector<u16> skybox_indices = {
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

	m_skybox_mesh = MeshMgr::get_singleton()->create_mesh(skybox_vertices, skybox_indices);
}

void RenderingMgr::render_skybox()
{
	auto* renderer = Root::get_singleton()->renderer_backend();

	renderer->bind_shader(m_vertex_shader);
	renderer->bind_shader(m_skybox_fragment);

	renderer->set_texture(0, m_skybox_texture);
	renderer->set_sampler(0, m_skybox_sampler);

	renderer->render({
		.mesh = m_skybox_mesh,
		.model_matrix = Mat3x4::identity(),
		.view_matrix = Root::get_singleton()->main_camera.view_matrix().basis(),
		.proj_matrix = Root::get_singleton()->main_camera.proj_matrix(),
		.target = nullptr
	});
}

void RenderingMgr::render_actors()
{
	auto* renderer = Root::get_singleton()->renderer_backend();

	renderer->bind_shader(m_vertex_shader);
	renderer->bind_shader(m_fragment_shader);

	// instancing?
	act::ActorMgr::get_singleton()->foreach([&](act::ActorHandle& handle) -> void {
		if (!handle->get_model()) {
			return;
		} else {
			auto* model = handle->get_model();

			renderer->set_texture(0, model->material().texture());
			renderer->set_sampler(0, model->material().sampler());

			renderer->render({
				.mesh = model->mesh(),
				.model_matrix = model->transform().matrix() * handle->get_transform().matrix(),
				.view_matrix = Root::get_singleton()->main_camera.view_matrix(), // todo: combine the two matrices into one matrix???
				.proj_matrix = Root::get_singleton()->main_camera.proj_matrix(),
				.target = nullptr
			});
		}
	});
}

void RenderingMgr::render_scene_and_swap_buffers()
{
	auto* renderer = Root::get_singleton()->renderer_backend();

	renderer->set_clear_colour(wvn::Colour::black());

	renderer->begin_render();
	renderer->set_depth_params(true, false);
	render_skybox();
	renderer->set_depth_params(true, true);
	render_actors();
	renderer->end_render();

	renderer->swap_buffers();
}

Model* RenderingMgr::create_model()
{
	Model* m = new Model();
	m_models.push_back(m);
	return m;
}

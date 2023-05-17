#include <wvn/graphics/rendering_mgr.h>
#include <wvn/graphics/renderer_backend.h>
#include <wvn/graphics/texture_mgr.h>
#include <wvn/graphics/shader_mgr.h>
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
{
	m_vertex_shader = ShaderMgr::get_singleton()->create("../test/res/vert.spv", SHADER_TYPE_VERTEX);
	m_fragment_shader = ShaderMgr::get_singleton()->create("../test/res/frag.spv", SHADER_TYPE_FRAGMENT);

	dev::LogMgr::get_singleton()->print("[RENDERING] Initialized!");
}

RenderingMgr::~RenderingMgr()
{
	for (auto& m : m_models) {
		delete m;
	}

	dev::LogMgr::get_singleton()->print("[RENDERING] Destroyed!");
}

void RenderingMgr::render_scene()
{
	auto* renderer = Root::get_singleton()->renderer_backend();

	static float r = 0.0f;
	r += time::delta;

	renderer->bind_shader(m_vertex_shader);
	renderer->bind_shader(m_fragment_shader);

	act::ActorMgr::get_singleton()->foreach([&](act::ActorHandle& handle) -> void {
		if (!handle->get_model()) {
			return;
		} else {
			auto* model = handle->get_model();

			renderer->set_texture(0, model->material().texture());
			renderer->set_sampler(0, model->material().sampler());

			renderer->render({
				 .clear_colour = Colour::black(),
				 .mesh = model->mesh(),
				 .model_matrix = model->transform().matrix() * handle->get_transform().matrix(),
				 .view_matrix = Root::get_singleton()->main_camera.view_matrix(), // todo: combine the two matrices into one matrix???
				 .proj_matrix = Root::get_singleton()->main_camera.proj_matrix(),
				 .target = nullptr
			 });
		}
	});
}

void RenderingMgr::swap_buffers()
{
	Root::get_singleton()->renderer_backend()->swap_buffers();
}

Model* RenderingMgr::create_model()
{
	Model* m = new Model();
	m_models.push_back(m);
	return m;
}

#include <wvn/graphics/rendering_mgr.h>
#include <wvn/graphics/renderer_backend.h>
#include <wvn/graphics/texture_mgr.h>
#include <wvn/input/input_mgr.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/root.h>
#include <wvn/time.h>

using namespace wvn;
using namespace wvn::gfx;

WVN_IMPL_SINGLETON(RenderingMgr);

RenderingMgr::RenderingMgr()
	: m(nullptr), t(nullptr), s(nullptr)
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
	t = TextureMgr::get_singleton()->create("../images/kitty.png");
	s = TextureMgr::get_singleton()->create_sampler(gfx::TEX_FILTER_LINEAR);

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

	camera_stuff();

	auto* renderer = Root::get_singleton()->renderer_backend();

	renderer->set_texture(0, t);
	renderer->set_sampler(0, s);

	renderer->render({
		.clear_colour = Colour::black(),
		.camera = &Root::get_singleton()->main_camera,
		.mesh = m,
		.model_matrix = Mat4x4::create_rotation(r, Vec3F::up()) * Mat4x4::create_translation(0.0f, 0.0f, 5.0f)
	});
}

void RenderingMgr::swap_buffers()
{
	Root::get_singleton()->renderer_backend()->swap_buffers();
}

void RenderingMgr::camera_stuff()
{
	auto& camera = Root::get_singleton()->main_camera;

	static float t = 0.0f;

	if (inp::InputMgr::get_singleton()->is_down(inp::KEY_Q)) {
		t -= 0.0125f;
	} else if (inp::InputMgr::get_singleton()->is_down(inp::KEY_E)) {
		t += 0.0125f;
	}

	Vec3F direction = Vec3F::from_angle(0.0f, -t + CalcF::PI / 2.0f, 1.0f);

	camera.direction = Quaternion::from_axis_angle(direction.rotate(-CalcF::PI / 2.0f, Vec3F::up()), t);

	if (inp::InputMgr::get_singleton()->is_down(inp::KEY_A)) {
		camera.transform.move(-(-Vec3F::cross(direction, Vec3F::up()) * 0.025f)); // negative due to left-handed coordinate system
	} else if (inp::InputMgr::get_singleton()->is_down(inp::KEY_D)) {
		camera.transform.move(  -Vec3F::cross(direction, Vec3F::up()) * 0.025f); // negative due to left-handed coordinate system
	}

	if (inp::InputMgr::get_singleton()->is_down(inp::KEY_SPACE)) {
		camera.transform.move_y(-0.025f); // todo: this should be positive?
	} else if (inp::InputMgr::get_singleton()->is_down(inp::KEY_LEFT_SHIFT)) {
		camera.transform.move_y(0.025f);
	}

	if (inp::InputMgr::get_singleton()->is_down(inp::KEY_S)) {
		camera.transform.move(-direction * 0.025f);
	} else if (inp::InputMgr::get_singleton()->is_down(inp::KEY_W)) {
		camera.transform.move( direction * 0.025f);
	}
}

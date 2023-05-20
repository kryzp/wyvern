#include <wvn/root.h>
#include <wvn/container/vector.h>
#include <wvn/input/input_mgr.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/actor/actor.h>
#include <wvn/actor/actor_mgr.h>
#include <wvn/actor/event.h>
#include <wvn/actor/event_mgr.h>
#include <wvn/graphics/model.h>
#include <wvn/graphics/vertex.h>
#include <wvn/graphics/texture_mgr.h>
#include <wvn/graphics/mesh_mgr.h>
#include <wvn/graphics/rendering_mgr.h>
#include <wvn/system/system_backend.h>
#include <wvn/time.h>

/*
 * If you're wondering why theres seemingly so many todo's
 * then its because I'm pretty much learning as I go, so it's
 * essentially in a constant state of being prototyped.
 */

using Vertices = wvn::Vector<wvn::gfx::Vertex>;
using Indices = wvn::Vector<u16>;

class CameraController : public wvn::act::Actor
{
private:
	wvn::act::ActorHandle m_cube_handle;

	void movement_keyboard()
	{
		const float move_speed = 0.025f;

		auto& camera = wvn::Root::get_singleton()->main_camera;
		auto* inp = wvn::inp::InputMgr::get_singleton();

		wvn::Vec3F v1 = wvn::Vec3F::cross(camera.direction, wvn::Vec3F::up()).normalized();
		wvn::Vec3F v2 = wvn::Vec3F::cross(v1, camera.direction).normalized();

		if (inp->is_down(wvn::inp::KEY_A)) {
			camera.transform.move( v1 * move_speed); // remember: left-handed coordinate system, so naturally the left-hand rule is used for cross products!
		} else if (inp->is_down(wvn::inp::KEY_D)) {
			camera.transform.move(-v1  * move_speed);
		}

		if (inp->is_down(wvn::inp::KEY_SPACE)) {
			camera.transform.move( v2 * move_speed);
		} else if (inp->is_down(wvn::inp::KEY_LEFT_SHIFT)) {
			camera.transform.move(-v2 * move_speed);
		}

		if (inp->is_down(wvn::inp::KEY_W)) {
			camera.transform.move( camera.direction * move_speed);
		} else if (inp->is_down(wvn::inp::KEY_S)) {
			camera.transform.move(-camera.direction * move_speed);
		}
	}

	void movement_mouse()
	{
		auto* inp = wvn::inp::InputMgr::get_singleton();
		auto& camera = wvn::Root::get_singleton()->main_camera;

		float sensitivity = 0.003f;
		float dx = (float)(inp->mouse_position().x - wvn::Root::get_singleton()->config().width / 2.0f);
		float dy = (float)(inp->mouse_position().y - wvn::Root::get_singleton()->config().height / 2.0f);

		if ((dx * dx) + (dy * dy) > 0.5f) {
			tgtt += dx * sensitivity;
			tgts += dy * sensitivity;
		}

		t = wvn::CalcF::lerp(t, tgtt, 0.4f);
		s = wvn::CalcF::lerp(s, tgts, 0.4f);

		wvn::Vec3F direction = wvn::Vec3F::from_angle(-s, -t + wvn::CalcF::PI / 2.0f, 1.0f);
		camera.direction = direction;
	}

public:
	CameraController(const wvn::act::ActorHandle& cube)
		: wvn::act::Actor()
		, m_cube_handle(cube)
	{
	}

	void init() override
	{
		wvn::Root::get_singleton()->system_backend()->lock_cursor(true);
	}

	float t = 0;
	float s = 0;

	float tgtt = 0;
	float tgts = 0;

	void tick() override
	{
		auto* inp = wvn::inp::InputMgr::get_singleton();

		if (inp->is_down(wvn::inp::KEY_ESCAPE)) {
			wvn::Root::get_singleton()->exit();
		}

		if (inp->is_pressed(wvn::inp::KEY_G)) {
			wvn::act::Event("pingpong").send(m_cube_handle);
		}

		movement_mouse();
		movement_keyboard();


		wvn::Root::get_singleton()->system_backend()->set_cursor_position(1280 / 2, 720 / 2);
	}
};

class Cube : public wvn::act::Actor
{
	wvn::Vec3F m_velocity;
	wvn::Vec3F m_angular_velocity;

public:
	Cube(const Vertices& vv, const Indices& ii)
		: wvn::act::Actor()
		, m_velocity(0, 0, 0)
	{
		p_model = wvn::gfx::RenderingMgr::get_singleton()->create_model();
		p_model->mesh(wvn::gfx::MeshMgr::get_singleton()->create_mesh(vv, ii));
	}

	~Cube() override
	{
	}

	void init() override
	{
		// set model
		p_model->material().texture(wvn::gfx::TextureMgr::get_singleton()->create("../test/res/kitty.png"));
		p_model->material().sampler(wvn::gfx::TextureMgr::get_singleton()->create_sampler(wvn::gfx::TEX_FILTER_LINEAR));

		// set transform
		p_transform.position(0.0f, 0.0f, 5.0f);
		p_transform.rotation(wvn::Vec3F::up(), 0.0f);
		p_transform.scale(1.0f, 1.0f, 1.0f);
		p_transform.origin(0.0f, 0.0f, 0.0f);
	}

	void tick() override
	{
		p_transform.move(m_velocity * 0.1f);
		p_transform.rotate(m_angular_velocity.normalized(), m_angular_velocity.length_squared() * 0.2f);

		m_velocity = wvn::Vec3F::lerp(m_velocity, wvn::Vec3F::zero(), 0.05f);
		m_angular_velocity = wvn::Vec3F::lerp(m_angular_velocity, wvn::Vec3F::zero(), 0.05f);

		//p_transform.rotate(wvn::Vec3F::up(), 0.01f);
	}

	bool on_event(wvn::act::Event& event) override
	{
		if (wvn::act::Actor::on_event(event)) {
			return true;
		}

		if (event.is_type("pingpong"))
		{
			m_velocity = wvn::Vec3F::from_angle(
				wvn::Root::get_singleton()->random.real32(0, wvn::CalcF::TAU),
				wvn::Root::get_singleton()->random.real32(0, wvn::CalcF::TAU),
				1.0f
			);

			m_angular_velocity = wvn::Vec3F::from_angle(
				wvn::Root::get_singleton()->random.real32(0, wvn::CalcF::TAU),
				wvn::Root::get_singleton()->random.real32(0, wvn::CalcF::TAU),
				wvn::Root::get_singleton()->random.real32(0.5f, 1.5f)
			);

//			wvn::act::ActorHandle handle(this);
//			wvn::act::ActorMgr::get_singleton()->destroy(handle);
//			wvn::dev::LogMgr::get_singleton()->print(event.args["message.quit"].data.string);

			return true;
		}

		return false;
	}
};

int main()
{
	Vertices cube_vertices = {
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
		{ {  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
		{ { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
		{ {  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
		{ { -0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
	};

	Indices cube_indices = {
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

	wvn::Config cfg;
	{
		cfg.name = "The ethereal cube welcomes you.";
		cfg.width = 1280;
		cfg.height = 720;
		cfg.target_fps = 144;
		cfg.max_updates = 5;
		cfg.window_mode = wvn::WINDOW_MODE_WINDOWED;
		cfg.flags =
			wvn::Config::FLAG_CURSOR_VISIBLE |
			wvn::Config::FLAG_VSYNC |
			wvn::Config::FLAG_CENTRE_WINDOW;
	}

	new wvn::Root(cfg);
	{
		// lock mouse to center of window
		wvn::Root::get_singleton()->system_backend()->set_cursor_position(1280 / 2, 720 / 2);

		auto* root = wvn::Root::get_singleton();
		auto* act  = wvn::act::ActorMgr::get_singleton();
		auto& cam  = root->main_camera;

		cam.fov  = 45.0f;
		cam.near =  0.1f;
		cam.far  = 10.0f;

		auto cb = act->create<Cube>(cube_vertices, cube_indices);
		auto cb2 = act->create<Cube>(cube_vertices, cube_indices);
		auto db = act->create<CameraController>(cb);

		root->run();
	}
	delete wvn::Root::get_singleton();

	return 0;
}

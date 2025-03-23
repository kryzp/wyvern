#include <wvn/graphics/rendering_mgr.h>
#include <wvn/graphics/renderer_backend.h>
#include <wvn/graphics/texture_mgr.h>
#include <wvn/graphics/render_target_mgr.h>
#include <wvn/graphics/material_system.h>
#include <wvn/graphics/mesh_mgr.h>
#include <wvn/graphics/mesh.h>
#include <wvn/graphics/light.h>
#include <wvn/input/input.h>
#include <wvn/devenv/log_mgr.h>
#include <wvn/entity/entity_mgr.h>
#include <wvn/root.h>
#include <wvn/camera.h>

using namespace wvn;
using namespace wvn::gfx;

// todo: remove when finished the project for the love of god fucking hell this is so fucking shitty oh my fucking go krill yourslef (one in a krillion1) im going to ram a massive chalk brick into your face
#define backend (Root::get_singleton()->renderer_backend())
#define maincam (Root::get_singleton()->main_camera)

wvn_IMPL_SINGLETON(RenderingMgr);

RenderingMgr::RenderingMgr()
	: m_backbuffer()
	, m_skybox_texture()
	, m_skybox_sampler()
	, m_skybox_mesh()
	, m_curr_light_id()
	, m_lights()
	, m_light_shadow_sampler(nullptr)
	, m_curr_object_id()
	, m_objects()
{
	m_backbuffer = backend->create_backbuffer();
	m_backbuffer->set_clear_colour(Colour::black());

	MaterialSystem::get_singleton()->load_default_techniques();

	// shadow map sampler
	m_light_shadow_sampler = TextureMgr::get_singleton()->register_sampler(
		"light_sampler",
		TextureSampler::Style(
			TEX_FILTER_LINEAR,
			TEX_WRAP_CLAMP, TEX_WRAP_CLAMP, TEX_WRAP_CLAMP,
			TEX_BORDER_COLOUR_INT_OPAQUE_WHITE
		)
	);

	create_skybox();

	// todo: temp, add a light
	auto it = create_light(true);
	it->set_shadow_map(RenderTargetMgr::get_singleton()->get_depth_only_target(Light::DEPTH_TEXTURE_WIDTH, Light::DEPTH_TEXTURE_HEIGHT));
	it->set_type(LIGHT_TYPE_DIR);
	it->set_colour(Colour::white());
	it->toggle_ambient(true);
	it->set_ambient(Colour::white() * 0.05f);
	it->set_shadow_near_clip(1.0f);
	it->set_shadow_far_clip(12.0f);
	it->set_falloff(10.0f);
	it->set_intensity(10.0f);
	it->set_orientation(Vec3F(-1.0f, -1.0f, -1.0f).normalized());
	it->set_position(Vec3F(4.0f, 4.0f, 4.0f));

	dev::LogMgr::get_singleton()->print("[RENDERING] Initialized!");
}

RenderingMgr::~RenderingMgr()
{
	dev::LogMgr::get_singleton()->print("[RENDERING] Destroyed!");
}

RenderableObjectHandle RenderingMgr::create_renderable()
{
	RenderableObjectID id = ++m_curr_object_id;
	RenderableObject* obj = new RenderableObject(id);
	m_objects.insert(Pair(id, obj));
	return RenderableObjectHandle(obj);
}

bool RenderingMgr::is_valid_object(const RenderableObjectHandle& obj)
{
	if (obj.id() == RenderableObject::NULL_ID) {
		return false;
	}

	return m_objects.contains(obj.id());
}

RenderableObject* RenderingMgr::fetch_object(const RenderableObjectHandle& obj)
{
	return m_objects[obj.id()];
}

LightHandle RenderingMgr::create_light(bool is_shadow_caster)
{
	LightID id = ++m_curr_light_id;
	Light* light = new Light(id);
	light->toggle_shadows(is_shadow_caster);
	m_lights.insert(Pair(id, light));
	return LightHandle(light);
}

bool RenderingMgr::is_valid_light(const LightHandle& light)
{
	if (light.id() == Light::NULL_ID) {
		return false;
	}

	return m_lights.contains(light.id());
}

Light* RenderingMgr::fetch_light(const LightHandle& light)
{
	return m_lights[light.id()];
}

Camera RenderingMgr::get_light_camera(const Light& light) const
{
	Camera light_camera(Camera::CAM_ORTHO, 5.0f, 5.0f);
	light_camera.up = Vec3F::up();
	light_camera.near = light.get_shadow_near_clip();
	light_camera.far = light.get_shadow_far_clip();
	light_camera.direction = light.get_orientation().vector();
	light_camera.position = light.get_position();

	return light_camera;
}

void RenderingMgr::render_scene_and_swap_buffers()
{
	// push constants must be initialized *IN THE ORDER* that they appear in the shader
	// on initial initialisation, they should be added in the order they appear in the uniform buffer of the shader
	ShaderParameters push_constants;
	push_constants.set("view", Mat4x4::identity());
	push_constants.set("proj", Mat4x4::identity());
	push_constants.set("light_view", Mat4x4::identity());
	push_constants.set("light_proj", Mat4x4::identity());
	push_constants.set("camera_position_and_time", { maincam.position.x, maincam.position.y, maincam.position.z, (float)time::elapsed });

	// iterate through all shadow-casting lights
	// and update their shadow maps to reflect the scene
	perform_shadow_pass(push_constants);

	// render the world to the actual game window
	backend->set_render_target(m_backbuffer);
	backend->set_cull_mode(CULL_MODE_BACK);
	backend->begin_render();
	perform_forward_pass(push_constants);
	backend->end_render();

//	backend->set_render_target(m_backbuffer);
//	backend->set_cull_mode(CULL_MODE_BACK);
//	backend->begin_render();
//	perform_forward_pass(push_constants);
//	backend->end_render();

//	push_constants.set("view", Mat4x4::identity());
//	push_constants.set("proj", Mat4x4::identity());
//	push_constants.set("light_view", Mat4x4::identity());
//	push_constants.set("light_proj", Mat4x4::identity());
//	push_constants.set("camera_position_and_time", { maincam.position.x, maincam.position.y, maincam.position.z, time::elapsed });
//
//	backend->set_push_constants(push_constants);
//	backend->set_render_target(m_backbuffer);
//	backend->set_depth_params(false, false);
//	backend->set_cull_mode(CULL_MODE_BACK);
//	backend->begin_render();
//	primitive_forward_render(m_fbo_output_mesh);
//	backend->end_render();

	backend->reset_push_constants();

	backend->swap_buffers();
}

void RenderingMgr::perform_forward_pass(ShaderParameters& push_constants)
{
	push_constants.set("view", maincam.view_matrix().basis());
	push_constants.set("proj", maincam.proj_matrix());
	backend->set_push_constants(push_constants);

	backend->set_depth_params(false, false);

	render_mesh(
		SHADER_PASS_FORWARD,
		m_skybox_mesh,
		Affine3D::identity()
	);

	push_constants.set("view", maincam.view_matrix());
	backend->set_push_constants(push_constants);

	backend->set_depth_params(true, true);

	for (auto& [id, obj] : m_objects)
	{
		if (obj->mesh)
		{
			Camera cam = get_light_camera(*m_lights.first()->data.second);
			push_constants.set("light_view", cam.view_matrix());
			push_constants.set("light_proj", cam.proj_matrix());
			backend->set_push_constants(push_constants);

			obj->mesh->submesh(0)->material()->set_texture(1,
				m_skybox_texture,
				m_skybox_sampler
			);

			obj->mesh->submesh(0)->material()->set_texture(2,
				m_lights.first()->data.second->get_shadow_map()->get_depth_attachment(),
				m_light_shadow_sampler
			);

			render_mesh(
				SHADER_PASS_FORWARD,
				obj->mesh,
				obj->matrix
			);
		}
	}
}

void RenderingMgr::perform_shadow_pass(ShaderParameters& push_constants)
{
	for (auto& [id, light] : m_lights)
	{
		if (!light->is_shadow_caster()) {
			continue;
		}

		backend->set_render_target(light->get_shadow_map());
		backend->set_cull_mode(CULL_MODE_FRONT);
		backend->begin_render();

		Camera cam = get_light_camera(*light);
		perform_single_shadow_pass(push_constants, cam);

		backend->end_render();
	}
}

void RenderingMgr::perform_single_shadow_pass(ShaderParameters& push_constants, const Camera& camera)
{
	push_constants.set("view", camera.view_matrix());
	push_constants.set("proj", camera.proj_matrix());
	backend->set_push_constants(push_constants);

	backend->set_depth_params(true, true);

	for (auto& [obj_id, obj] : m_objects)
	{
		if (!obj->mesh) {
			continue;
		}

		render_mesh(
			SHADER_PASS_SHADOW,
			obj->mesh,
			obj->matrix
		);
	}
}

void RenderingMgr::render_mesh(int pass_id, const Mesh* mesh, const Affine3D& model_matrix)
{
	for (int i = 0; i < mesh->submesh_count(); i++)
	{
		const SubMesh* submesh = mesh->submesh(i);
		const Material* material = submesh->material();

		if (pass_id == SHADER_PASS_SHADOW) {
			for (int j = 0; j < material->textures.size(); j++) { // todo: figure out why i need to call this... at all. shouldn't the shadow pass be an empty material that would reset all textures anyways?
				backend->set_texture(j, nullptr);
			}
		} else {
			for (int j = 0; j < material->textures.size(); j++) {
				backend->set_texture(j, material->textures[j].texture);
				if (material->textures[j].sampler) {
					backend->set_sampler(j, material->textures[j].sampler);
				}
			}
		}

		Mat4x4 normal_matrix = model_matrix.basis.inverse().transpose().as4x4();
		auto* shader = material->technique.get_pass(pass_id);

		for (int k = 0; k < shader->stages.size(); k++)
		{
			shader->stages[k]->params.set("model", model_matrix.build_transformation_matrix());
			shader->stages[k]->params.set("normal_matrix", normal_matrix);

			backend->bind_shader(shader->stages[k]);
			backend->bind_shader_params(shader->stages[k]->type, shader->stages[k]->params);
		}

		RenderPass pass;
		pass.mesh = submesh;

		backend->render(pass.build());
	}
}

void RenderingMgr::primitive_forward_render(const Mesh* mesh)
{
	for (int i = 0; i < mesh->submesh_count(); i++)
	{
		const SubMesh* submesh = mesh->submesh(i);
		const Material* material = submesh->material();

		for (int j = 0; j < material->textures.size(); j++)
		{
			backend->set_texture(j, material->textures[j].texture);

			if (material->textures[j].sampler) {
				backend->set_sampler(j, material->textures[j].sampler);
			}
		}

		auto* shader = material->technique.get_pass(SHADER_PASS_FORWARD);

		for (int k = 0; k < shader->stages.size(); k++)
		{
			shader->stages[k]->params.set("model", Mat4x4::identity());
			shader->stages[k]->params.set("normal_matrix", Mat4x4::identity());

			backend->bind_shader(shader->stages[k]);
			backend->bind_shader_params(shader->stages[k]->type, shader->stages[k]->params);
		}

		RenderPass pass;
		pass.mesh = submesh;

		backend->render(pass.build());
	}
}

void RenderingMgr::create_skybox()
{
	Image img_rt("../res/skyboxes/skybox6/posx.jpg");
	Image img_lf("../res/skyboxes/skybox6/negx.jpg");
	Image img_tp("../res/skyboxes/skybox6/posy.jpg");
	Image img_bt("../res/skyboxes/skybox6/negy.jpg");
	Image img_ft("../res/skyboxes/skybox6/negz.jpg");
	Image img_bk("../res/skyboxes/skybox6/posz.jpg");

	m_skybox_texture = TextureMgr::get_singleton()->register_cube_map("skybox",
		TEX_FORMAT_R8G8B8A8_SRGB,
		img_rt,
		img_lf,
		img_tp,
		img_bt,
		img_ft,
		img_bk
	);

	m_skybox_sampler = TextureMgr::get_singleton()->register_sampler("skybox_sampler", TEX_FILTER_LINEAR);

	Vector<Vertex> skybox_vertices = {
		{ { -1.0,  1.0,  1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
		{ { -1.0, -1.0,  1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
		{ {  1.0, -1.0,  1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
		{ {  1.0,  1.0,  1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
		{ { -1.0,  1.0, -1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
		{ { -1.0, -1.0, -1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
		{ {  1.0, -1.0, -1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
		{ {  1.0,  1.0, -1.0 }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } }
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

	MaterialData material_data;
	material_data.textures[0] = SampledTexture(m_skybox_texture, m_skybox_sampler);
	material_data.technique = "skybox";

	m_skybox_mesh = MeshMgr::get_singleton()->create_mesh();
	auto* skybox = m_skybox_mesh->create_submesh();
	skybox->build(skybox_vertices, skybox_indices);
	skybox->set_material(MaterialSystem::get_singleton()->build_material(material_data));
}

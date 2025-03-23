#include <wvn/graphics/material_system.h>
#include <wvn/graphics/shader_mgr.h>
#include <wvn/devenv/log_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

wvn_IMPL_SINGLETON(MaterialSystem);

MaterialSystem::MaterialSystem()
{
	dev::LogMgr::get_singleton()->print("[MATERIAL] Initialized!");
}

void MaterialSystem::load_default_techniques()
{
	ShaderProgram* shd_generic_vtx   		= ShaderMgr::get_singleton()->get_shader("../res/vertex.spv", SHADER_TYPE_VERTEX);
	ShaderProgram* shd_fragment_box   		= ShaderMgr::get_singleton()->get_shader("../res/fragment.spv", SHADER_TYPE_FRAGMENT);
	ShaderProgram* shd_out_fragment   		= ShaderMgr::get_singleton()->get_shader("../res/out_fragment.spv", SHADER_TYPE_FRAGMENT);
	ShaderProgram* shd_sky_fragment   		= ShaderMgr::get_singleton()->get_shader("../res/skybox_frag.spv", SHADER_TYPE_FRAGMENT);
	ShaderProgram* shd_update_depth_frag 	= ShaderMgr::get_singleton()->get_shader("../res/update_depth_fragment.spv", SHADER_TYPE_FRAGMENT);
	ShaderProgram* shd_draw_depth_frag		= ShaderMgr::get_singleton()->get_shader("../res/draw_depth_fragment.spv", SHADER_TYPE_FRAGMENT);

	ShaderEffect* depth_update_effect = ShaderMgr::get_singleton()->build_effect();
	depth_update_effect->add_stage(shd_generic_vtx);
	depth_update_effect->add_stage(shd_update_depth_frag);

	ShaderEffect* depth_draw_effect = ShaderMgr::get_singleton()->build_effect();
	depth_draw_effect->add_stage(shd_generic_vtx);
	depth_draw_effect->add_stage(shd_draw_depth_frag);

	ShaderEffect* forward_effect = ShaderMgr::get_singleton()->build_effect();
	forward_effect->add_stage(shd_generic_vtx);
	forward_effect->add_stage(shd_fragment_box);

	ShaderEffect* skybox_effect = ShaderMgr::get_singleton()->build_effect();
	skybox_effect->add_stage(shd_generic_vtx);
	skybox_effect->add_stage(shd_sky_fragment);

	ShaderEffect* output_fbo_effect = ShaderMgr::get_singleton()->build_effect();
	output_fbo_effect->add_stage(shd_generic_vtx);
	output_fbo_effect->add_stage(shd_out_fragment);

	Technique depth_draw_technique;
	Technique forward_technique;
	Technique skybox_technique;
	Technique output_fbo_technique;

	depth_draw_technique.set_pass(SHADER_PASS_SHADOW, depth_update_effect);
	depth_draw_technique.set_pass(SHADER_PASS_FORWARD, depth_draw_effect);

	forward_technique.set_pass(SHADER_PASS_SHADOW, depth_update_effect);
	forward_technique.set_pass(SHADER_PASS_FORWARD, forward_effect);

	skybox_technique.set_pass(SHADER_PASS_SHADOW, depth_update_effect);
	skybox_technique.set_pass(SHADER_PASS_FORWARD, skybox_effect);

	output_fbo_technique.set_pass(SHADER_PASS_SHADOW, depth_update_effect);
	output_fbo_technique.set_pass(SHADER_PASS_FORWARD, output_fbo_effect);

	add_technique("depth_draw", depth_draw_technique);
	add_technique("forward", forward_technique);
	add_technique("skybox", skybox_technique);
	add_technique("output_fbo", output_fbo_technique);

	dev::LogMgr::get_singleton()->print("[MATERIAL] Loaded default techniques!");
}

MaterialSystem::~MaterialSystem()
{
	for (auto& mat : m_materials) {
		delete mat;
	}

	dev::LogMgr::get_singleton()->print("[MATERIAL] Destroyed!");
}

Material* MaterialSystem::build_material(const MaterialData& data)
{
	Material* material = new Material();

	material->textures = data.textures;
	material->technique = m_techniques[data.technique];

	m_materials.push_back(material);
	return material;
}

void MaterialSystem::add_technique(const String &name, const Technique &technique)
{
	m_techniques.insert(Pair(name, technique));
}

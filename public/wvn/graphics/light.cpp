#include <wvn/graphics/light.h>
#include <wvn/graphics/material_system.h>
#include <wvn/graphics/render_target_mgr.h>
#include <wvn/graphics/texture_mgr.h>
#include <wvn/graphics/rendering_mgr.h>

using namespace wvn;
using namespace wvn::gfx;

Light::Light(LightID id)
	: m_type(LIGHT_TYPE_DIR)
	, m_id(id)
	, m_colour(Colour::white())
	, m_ambient(Colour::white() * 0.05f)
	, m_ambient_enabled(true)
	, m_shadows_enabled(true)
	, m_shadow_near_clip(0.1f)
	, m_shadow_far_clip(100.0f)
	, m_shadow_map(nullptr)
	, m_intensity(1.0f)
	, m_falloff(1.0f)
	, m_orientation()
	, m_position()
{
}

Light::~Light()
{
}

void Light::set_type(LightType type)
{
	m_type = type;
}

LightType Light::get_type() const
{
	return m_type;
}

void Light::set_colour(const Colour& colour)
{
	m_colour = colour;
}

const Colour& Light::get_colour() const
{
	return m_colour;
}

void Light::toggle_ambient(bool enabled)
{
	m_ambient_enabled = enabled;
}

bool Light::is_ambient_enabled() const
{
	return m_ambient_enabled;
}

void Light::set_ambient(const Colour& colour)
{
	m_ambient = colour;
}

const Colour& Light::get_ambient() const
{
	return m_ambient;
}

void Light::toggle_shadows(bool enabled)
{
	m_shadows_enabled = enabled;
}

bool Light::is_shadow_caster() const
{
	return m_shadows_enabled;
}

RenderTarget* Light::get_shadow_map() const
{
	return m_shadow_map;
}

void Light::set_shadow_map(RenderTarget* target)
{
	m_shadow_map = target;
}

void Light::set_shadow_near_clip(float distance)
{
	m_shadow_near_clip = distance;
}

float Light::get_shadow_near_clip() const
{
	return m_shadow_near_clip;
}

void Light::set_shadow_far_clip(float distance)
{
	m_shadow_far_clip = distance;
}

float Light::get_shadow_far_clip() const
{
	return m_shadow_far_clip;
}

void Light::set_falloff(float falloff)
{
	m_falloff = falloff;
}

float Light::get_falloff() const
{
	return m_falloff;
}

void Light::set_intensity(float intensity)
{
	m_intensity = intensity;
}

float Light::get_intensity() const
{
	return m_intensity;
}

const Quat& Light::get_orientation() const
{
	return m_orientation;
}

void Light::set_orientation(const Quat& orientation)
{
	m_orientation = orientation;
}

const Vec3F& Light::get_position() const
{
	return m_position;
}

void Light::set_position(const Vec3F& position)
{
	m_position = position;
}

///////////////////////////////////////////////

LightHandle::LightHandle()
	: m_id(RenderableObject::NULL_ID)
{
}

LightHandle::LightHandle(const Light* light)
	: m_id(light ? light->m_id : 0)
{
}

LightHandle::LightHandle(LightID id)
	: m_id(id)
{
}

LightHandle::LightHandle(const LightHandle& other)
	: m_id(other.m_id)
{
}

LightHandle::LightHandle(LightHandle&& other) noexcept
	: m_id(std::move(other.m_id))
{
	other.m_id = RenderableObject::NULL_ID;
}

LightHandle& LightHandle::operator = (const LightHandle& other)
{
	this->m_id = other.m_id;
	return *this;
}

LightHandle& LightHandle::operator = (LightHandle&& other) noexcept
{
	this->m_id = std::move(other.m_id);
	other.m_id = 0;
	return *this;
}

bool LightHandle::is_valid() const
{
	return RenderingMgr::get_singleton()->is_valid_light(*this);
}

LightHandle::operator bool () const
{
	return RenderingMgr::get_singleton()->is_valid_light(*this);
}

LightID LightHandle::id() const
{
	return m_id;
}

LightHandle::operator LightID () const
{
	return m_id;
}

Light* LightHandle::get()
{
	return RenderingMgr::get_singleton()->fetch_light(*this);
}

const Light* LightHandle::get() const
{
	return RenderingMgr::get_singleton()->fetch_light(*this);
}

Light* LightHandle::operator -> ()
{
	return RenderingMgr::get_singleton()->fetch_light(*this);
}

const Light* LightHandle::operator -> () const
{
	return RenderingMgr::get_singleton()->fetch_light(*this);
}

bool LightHandle::operator == (const LightHandle& other) const
{
	return (this->m_id == other.m_id);
}

bool LightHandle::operator != (const LightHandle& other) const
{
	return !(*this == other);
}

/*
///////////////////////////////////////////////

LightShadowMapMgr::LightShadowMapMgr()
	: m_light_shadow_textures{}
	, m_light_shadow_sampler(nullptr)
	, m_light_shadow_texture_num(0)
{
}

LightShadowMapMgr::~LightShadowMapMgr()
{
}

void LightShadowMapMgr::init()
{
}

Vector<ShadowCaster> LightShadowMapMgr::get_shadow_casters() const
{
	return {};
}

void LightShadowMapMgr::create_depth_texture()
{
	int idx = m_light_shadow_texture_num++;

	m_light_shadow_textures[idx] = RenderTargetMgr::get_singleton()->get_depth_target(Light::DEPTH_TEXTURE_WIDTH, Light::DEPTH_TEXTURE_HEIGHT);

///////////
	MaterialData material_data;
	material_data.textures[0] = SampledTexture(m_light_shadow_textures[idx]->get_depth_attachment(), m_light_shadow_sampler);
	material_data.technique = "depth_draw";

	m_fbo_output_mesh = MeshMgr::get_singleton()->create_mesh();

	auto* quad = m_fbo_output_mesh->create_submesh();
	quad->build({
		{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
		{ {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
		{ {  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }
	}, {
		0, 1, 2,
		1, 3, 2
	});

	quad->set_material(MaterialSystem::get_singleton()->build_material(material_data));
 ////////
}
*/

#include <wvn/graphics/material.h>

using namespace wvn;
using namespace wvn::gfx;

Material::Material()
	: m_texture(nullptr)
	, m_sampler(nullptr)
	, m_techniques()
{
}

Material::Material(Texture* tex, TextureSampler* smp)
	: m_texture(tex)
	, m_sampler(smp)
	, m_techniques()
{
}

Material::~Material()
{
}

void Material::add_technique(const Technique& tech)
{
	m_techniques.push_back(tech);
}

Material::Techniques& Material::techniques()
{
	return m_techniques;
}

const Material::Techniques& Material::techniques() const
{
	return m_techniques;
}

void Material::texture(Texture* tex)
{
	m_texture = tex;
}

Texture* Material::texture()
{
	return m_texture;
}

const Texture* Material::texture() const
{
	return m_texture;
}

void Material::sampler(TextureSampler* smp)
{
	m_sampler = smp;
}

TextureSampler* Material::sampler()
{
	return m_sampler;
}

const TextureSampler* Material::sampler() const
{
	return m_sampler;
}

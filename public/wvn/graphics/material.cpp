#include <wvn/graphics/material.h>

using namespace wvn;
using namespace wvn::gfx;

Material::Material()
{
}

Material::~Material()
{
}

u64 Material::hash() const
{
	u64 ret = 0;
	hash::combine(&ret, &technique);
	hash::combine(&ret, &textures);
	return ret;
}

void Material::set_texture(int idx, const Texture* texture, TextureSampler* sampler)
{
	textures[idx].texture = texture;
	textures[idx].sampler = sampler;
}

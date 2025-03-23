#include <wvn/graphics/texture.h>

using namespace wvn;
using namespace wvn::gfx;

Texture::Texture()
	: m_depth(1)
	, m_mipmapped(false)
{
}

Texture::~Texture()
{
}

SampledTexture::SampledTexture()
	: texture(nullptr)
	, sampler(nullptr)
{
}

SampledTexture::SampledTexture(const Texture* texture, TextureSampler* sampler)
	: texture(texture)
	, sampler(sampler)
{
}

SampledTexture::~SampledTexture()
{
}

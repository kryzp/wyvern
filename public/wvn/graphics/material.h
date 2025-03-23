#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <wvn/const.h>

#include <wvn/container/array.h>
#include <wvn/container/string.h>

#include <wvn/graphics/technique.h>
#include <wvn/graphics/texture.h>
#include <wvn/graphics/shader.h>

namespace wvn::gfx
{
	/**
	 * Compact representation of a material.
	 */
	struct MaterialData
	{
		Array<SampledTexture, wvn_MAX_BOUND_TEXTURES> textures;
		ShaderParameters parameters;
		String technique;
	};

	/**
	 * Material that packages together the required shaders, textures, and other
	 * information required for drawing a material.
	 */
	class Material
	{
	public:
		Material();
		virtual ~Material();

		u64 hash() const;

		void set_texture(int idx, const Texture* texture, TextureSampler* sampler);

		Technique technique;
		ShaderParameters parameters;
		Array<SampledTexture, wvn_MAX_BOUND_TEXTURES> textures;
	};
}

#endif // MATERIAL_H_

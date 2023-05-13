#ifndef MATERIAL_H
#define MATERIAL_H

#include <wvn/container/vector.h>
#include <wvn/graphics/technique.h>
#include <wvn/graphics/texture.h>
#include <wvn/graphics/shader.h>

namespace wvn::gfx
{
	/**
	 * Material that packages together the required shaders, textures, and other
	 * information required for drawing a material.
	 */
	class Material
	{
	public:
		using Techniques = Vector<Technique>;

		Material();
		Material(Texture* tex, TextureSampler* smp);
		virtual ~Material();

		void add_technique(const Technique& tech);
		Techniques& techniques();
		const Techniques& techniques() const;

		void texture(Texture* tex);
		Texture* texture();
		const Texture* texture() const;

		void sampler(TextureSampler* smp);
		TextureSampler* sampler();
		const TextureSampler* sampler() const;

	private:
		Techniques m_techniques;
		TextureSampler* m_sampler;
		Texture* m_texture;
	};
}

#endif // MATERIAL_H

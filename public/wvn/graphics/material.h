#ifndef MATERIAL_H
#define MATERIAL_H

#include <wvn/container/vector.h>
#include <wvn/graphics/technique.h>

namespace wvn::gfx
{
	/**
	 * Material that packages together the required shaders, textures, and other
	 * information required for drawing a material.
	 */
	class Material
	{
		using Techniques = Vector<Technique>;

	public:
		Material();
		virtual ~Material();

	private:
		Techniques m_techniques;
	};
}

#endif // MATERIAL_H

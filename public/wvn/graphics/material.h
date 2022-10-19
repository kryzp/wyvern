#pragma once

namespace wvn::gfx
{
	/*
	 * Material that packages together the required shaders, textures, and other
	 * information required for drawing a material.
	 */
	class Material
	{
	public:
		Material();
		virtual ~Material();
	};
}

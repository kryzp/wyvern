#ifndef RENDERING_MGR_H
#define RENDERING_MGR_H

#include <wvn/singleton.h>
#include <wvn/graphics/mesh.h>
#include <wvn/graphics/texture.h>
#include <wvn/graphics/material.h>

namespace wvn::gfx
{
	/**
	 * Responsible for actually rendering the scene and sending the required
	 * render passes to the rendering backend.
	 */
	class RenderingMgr : public Singleton<RenderingMgr>
	{
		WVN_DEF_SINGLETON(RenderingMgr);

	public:
		RenderingMgr();
		~RenderingMgr();

		void render_scene();
		void swap_buffers();

		Shader* vertex_shader;
		Shader* fragment_shader;
	};
}

#endif // RENDERING_MGR_H

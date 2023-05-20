#ifndef RENDERING_MGR_H
#define RENDERING_MGR_H

#include <wvn/singleton.h>
#include <wvn/container/vector.h>
#include <wvn/graphics/mesh.h>
#include <wvn/graphics/texture.h>
#include <wvn/graphics/material.h>
#include <wvn/graphics/model.h>

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

		void render_scene_and_swap_buffers();

		Model* create_model();

	private:
		void create_skybox();
		void render_skybox();

		void render_actors();

		Vector<Model*> m_models;

		Shader* m_vertex_shader;
		Shader* m_fragment_shader;

		Shader* m_skybox_fragment;
		Texture* m_skybox_texture;
		TextureSampler* m_skybox_sampler;
		Mesh* m_skybox_mesh;
	};
}

#endif // RENDERING_MGR_H

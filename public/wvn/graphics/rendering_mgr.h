#ifndef RENDERING_MGR_H_
#define RENDERING_MGR_H_

#include <wvn/singleton.h>
#include <wvn/container/hash_map.h>
#include <wvn/graphics/sub_mesh.h>
#include <wvn/graphics/texture.h>
#include <wvn/graphics/material.h>
#include <wvn/graphics/mesh.h>
#include <wvn/graphics/light.h>
#include <wvn/graphics/renderable_object.h>
#include <wvn/graphics/render_target.h>

namespace wvn { class Camera; }

namespace wvn::gfx
{
	/*
	 * Responsible for actually rendering the scene and sending the required
	 * render passes to the rendering backend.
	 */
	class RenderingMgr : public Singleton<RenderingMgr>
	{
		wvn_DEF_SINGLETON(RenderingMgr);

	public:
		RenderingMgr();
		~RenderingMgr();

		void render_scene_and_swap_buffers();

		RenderableObjectHandle create_renderable();
		bool is_valid_object(const RenderableObjectHandle& obj);
		RenderableObject* fetch_object(const RenderableObjectHandle& obj);

		LightHandle create_light(bool is_shadow_caster);
		bool is_valid_light(const LightHandle& light);
		Light* fetch_light(const LightHandle& light);

	private:
		void perform_forward_pass(ShaderParameters& push_constants);

		void perform_shadow_pass(ShaderParameters& push_constants);
		void perform_single_shadow_pass(ShaderParameters& push_constants, const Camera& camera);

		void create_skybox();

		void render_mesh(int pass_id, const Mesh* mesh, const Affine3D& model_matrix);
		void primitive_forward_render(const Mesh* mesh);

		Camera get_light_camera(const Light& light) const;

		RenderTarget* m_backbuffer;

		Texture* m_skybox_texture;
		TextureSampler* m_skybox_sampler;
		Mesh* m_skybox_mesh;

		LightID m_curr_light_id;
		HashMap<LightID, Light*> m_lights;
		Vector<Light*> m_shadow_casting_lights;
//		LightShadowMapMgr m_light_shadow_map_mgr;
		TextureSampler* m_light_shadow_sampler;

		RenderableObjectID m_curr_object_id;
		HashMap<RenderableObjectID, RenderableObject*> m_objects;
	};
}

#endif // RENDERING_MGR_H_

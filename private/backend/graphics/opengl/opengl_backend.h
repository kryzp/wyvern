#ifndef OPENGL_BACKEND_H
#define OPENGL_BACKEND_H

#include <wvn/util/types.h>
#include <wvn/graphics/renderer_backend.h>

namespace wvn::gfx
{
	class OpenGLBackend : public RendererBackend
	{
	public:
		OpenGLBackend();
		~OpenGLBackend() override;

		RendererBackendProperties properties() override;

		void render(const RenderPass& pass) override;

		void wait_for_sync() override;

		void on_window_resize(int width, int height) override;

		Texture* create_texture(u32 width, u32 height) override;
		RenderTarget* create_render_target(u32 width, u32 height) override;
		Mesh* create_mesh() override;
	};
}

#endif // OPENGL_BACKEND_H

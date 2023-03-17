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
	};
}

#endif // OPENGL_BACKEND_H

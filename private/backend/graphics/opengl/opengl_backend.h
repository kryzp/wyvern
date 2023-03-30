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
		void swap_buffers() override;

		void set_texture(u32 idx, const Texture* texture) override;
		void set_sampler(u32 idx, const TextureSampler& sampler) override;

		void on_window_resize(int width, int height) override;
	};
}

#endif // OPENGL_BACKEND_H

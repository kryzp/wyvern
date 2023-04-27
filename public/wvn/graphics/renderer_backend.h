#ifndef RENDERER_BACKEND_H
#define RENDERER_BACKEND_H

#include <wvn/graphics/renderer_backend.h>
#include <wvn/graphics/font.h>
#include <wvn/graphics/image.h>
#include <wvn/graphics/material.h>
#include <wvn/graphics/mesh.h>
#include <wvn/graphics/render_pass.h>
#include <wvn/graphics/render_target.h>
#include <wvn/graphics/texture.h>

#include <wvn/maths/colour.h>

#include <wvn/container/vector.h>

namespace wvn::gfx
{
	/**
	 * Properties a rendering backend might have that are exclusive to itself.
	 */
	struct RendererBackendProperties
	{
		bool origin_bottom_left;
	};

	/**
	 * Renderer backend that can be implemented by specific API's to
	 * disconnect them from being directly connected to the actual game
	 * engine, effectively acting as an interface.
	 */
	class RendererBackend
	{
	public:
		RendererBackend() = default;
		virtual ~RendererBackend() = default;

		virtual RendererBackendProperties properties() = 0;

		virtual void render(const RenderPass& pass) = 0;
		virtual void swap_buffers() = 0;

		virtual void set_texture(u32 idx, const Texture* texture) = 0;
		virtual void set_sampler(u32 idx, TextureSampler* sampler) = 0;

		virtual void on_window_resize(int width, int height) = 0;
	};
}

#endif // RENDERER_BACKEND_H

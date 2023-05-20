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

#define WVN_MAX_BOUND_TEXTURES 16

namespace wvn::gfx
{
	/**
	 * The UBO currently being sent to the shaders.
	 * todo: this should be modifiable! custom ubo's.
	 */
	struct UniformBufferObject
	{
		alignas(16) Mat4x4 model;
		alignas(16) Mat4x4 view;
		alignas(16) Mat4x4 proj;
	};

	/**
	 * Properties a rendering backend might have that are exclusive to itself.
	 */
	struct RendererBackendProperties
	{
		bool origin_bottom_left;
	};

	/**
	 * Renderer backend interface.
	 */
	class RendererBackend
	{
	public:
		RendererBackend() = default;
		virtual ~RendererBackend() = default;

		virtual RendererBackendProperties properties() = 0;

		virtual void begin_render() = 0;
		virtual void render(const RenderPass& pass) = 0;
		virtual void end_render() = 0;

		virtual void swap_buffers() = 0;
		virtual void set_clear_colour(const Colour& colour) = 0;

		virtual void set_depth_params(bool depth_test, bool depth_write) = 0;

		virtual void set_texture(u32 idx, const Texture* texture) = 0;
		virtual void set_sampler(u32 idx, TextureSampler* sampler) = 0;

		virtual void bind_shader(Shader* shader) = 0;

		virtual void on_window_resize(int width, int height) = 0;
	};
}

#endif // RENDERER_BACKEND_H

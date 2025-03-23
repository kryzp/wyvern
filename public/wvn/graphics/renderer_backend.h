#ifndef RENDERER_BACKEND_H_
#define RENDERER_BACKEND_H_

#include <wvn/graphics/font.h>
#include <wvn/graphics/image.h>
#include <wvn/graphics/material.h>
#include <wvn/graphics/sub_mesh.h>
#include <wvn/graphics/render_pass.h>
#include <wvn/graphics/render_target.h>
#include <wvn/graphics/backbuffer.h>
#include <wvn/graphics/texture.h>
#include <wvn/graphics/blend.h>

#include <wvn/maths/colour.h>

#include <wvn/container/vector.h>

namespace wvn::gfx
{
	enum CullMode
	{
		CULL_MODE_NONE = 0,
		CULL_MODE_BACK,
		CULL_MODE_FRONT,
		CULL_MODE_FRONT_AND_BACK,
		CULL_MODE_MAX_ENUM
	};

	/**
	 * Properties a rendering backend might have that are exclusive to itself.
	 */
	struct RendererBackendProperties
	{
		bool y_positive_down;
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
		virtual void render(const RenderOp& op) = 0;
		virtual void end_render() = 0;
		virtual void swap_buffers() = 0;

        virtual Backbuffer* create_backbuffer() = 0;
		virtual void set_render_target(RenderTarget* target) = 0;

		virtual void toggle_blending(bool enabled) = 0;
		virtual void set_blend_write_mask(bool r, bool g, bool b, bool a) = 0;
		virtual void set_blend_colour(const Blend& blend) = 0;
		virtual void set_blend_alpha(const Blend& blend) = 0;
		virtual void set_blend_constants(float r, float g, float b, float a) = 0;
		virtual void get_blend_constants(float* constants) = 0; // array of 4 floats!!
		virtual void set_blend_op(bool enabled, LogicOp op) = 0;

		virtual void set_depth_params(bool depth_test, bool depth_write) = 0;
		virtual void set_depth_op(CompareOp op) = 0;
		virtual void set_depth_bounds_test(bool enabled) = 0;
		virtual void set_depth_bounds(float min, float max) = 0;
		virtual void set_depth_stencil_test(bool enabled) = 0;

		virtual void set_viewport(const RectF& rect) = 0;
		virtual void set_scissor(const RectI& rect) = 0;

		virtual void set_sample_shading(bool enabled, float min_sample_shading) = 0;
		virtual void set_cull_mode(CullMode cull) = 0;

		virtual void set_texture(u32 idx, const Texture* texture) = 0;
		virtual void set_sampler(u32 idx, TextureSampler* sampler) = 0;

		void clear_texture(u32 idx) { set_texture(idx, nullptr); }

		virtual void bind_shader(const ShaderProgram* shader) = 0;
		virtual void bind_shader_params(ShaderProgramType shader_type, ShaderParameters& params) = 0;

		// some (vulkan) but not all rendering backends have support for "push constants"
		virtual void set_push_constants(ShaderParameters& params) = 0;
		virtual void reset_push_constants() = 0;

		virtual void on_window_resize(int width, int height) = 0;

	protected:
		RenderTarget* m_current_render_target;
	};
}

#endif // RENDERER_BACKEND_H_

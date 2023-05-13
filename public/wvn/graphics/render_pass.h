#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include <wvn/maths/colour.h>
#include <wvn/maths/rect.h>
#include <wvn/container/optional.h>
#include <wvn/camera.h>

#include <wvn/graphics/render_target.h>
#include <wvn/graphics/mesh.h>
#include <wvn/graphics/material.h>

namespace wvn::gfx
{
	/**
	 * Generic render pass that can be sent to the backend to be carried out
	 * and drawn to the screen.
	 */
	struct RenderPass
	{
		Optional<Colour> clear_colour;
		Optional<RectF> viewport;
		Optional<RectI> scissor;

		Mat4x4 model_matrix;
		Mat4x4 view_matrix;
		Mat4x4 proj_matrix;

		Mesh* mesh;
		RenderTarget* target;
	};
}

#endif // RENDER_PASS_H

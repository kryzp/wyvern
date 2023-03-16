#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include <wvn/maths/colour.h>
#include <wvn/maths/rect.h>
#include <wvn/container/optional.h>

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
		Colour clear_colour;

		Optional<RectF> viewport;
		Optional<RectI> scissor;

		Ref<Material> material;
		Ref<Mesh> mesh;
	};
}

#endif // RENDER_PASS_H

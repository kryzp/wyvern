#pragma once

#include <wvn/maths/colour.h>
#include <wvn/maths/rect.h>
#include <wvn/container/optional.h>

namespace wvn::gfx
{
	/*
	 * Generic render pass that can be sent to the backend to be carried out
	 * and drawn to the screen.
	 */
	struct RenderPass
	{
		Colour clear_colour;

		Optional<RectF> viewport;
		Optional<RectI> scissor;
	};
}

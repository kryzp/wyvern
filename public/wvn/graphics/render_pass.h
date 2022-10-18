#pragma once

#include <wvn/maths/colour.h>
#include <wvn/maths/rect.h>
#include <wvn/container/optional.h>

namespace wvn::gfx
{
	struct RenderPass
	{
		Colour clear_colour;

		Optional<RectF> viewport;
		Optional<RectI> scissor;
	};
}

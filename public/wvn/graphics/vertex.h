#pragma once

#include <wvn/maths/vec2.h>
#include <wvn/maths/colour.h>

namespace wvn::gfx
{
	/*
	 * Represents a vertex that can be drawn to the screen.
	 */
	struct Vertex
	{
		Vec2F pos;
		DisplayColour col;
	};
}

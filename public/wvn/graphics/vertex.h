#ifndef VERTEX_H
#define VERTEX_H

#include <wvn/maths/vec2.h>
#include <wvn/maths/colour.h>

namespace wvn::gfx
{
	/**
	 * Represents a vertex that can be drawn to the screen.
	 */
	struct Vertex
	{
		Vec3F pos;
		Vec2F uv;
		DisplayColour col;
	};
}

#endif // VERTEX_H

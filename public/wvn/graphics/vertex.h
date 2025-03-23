#ifndef VERTEX_H_
#define VERTEX_H_

#include <wvn/maths/vec3.h>
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
		Vec3F norm;
	};
}

#endif // VERTEX_H_

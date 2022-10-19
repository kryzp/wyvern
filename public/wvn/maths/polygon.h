#pragma once

#include <wvn/container/vector.h>
#include <functional>
#include <wvn/maths/vec2.h>

namespace wvn
{
	/*
	 * Representation of a polygon as a list of vertices.
	 */
	struct Polygon
	{
		Vector<Vec2F> vertices;
		
		Polygon();
		Polygon(const Vector<Vec2F>& vs);

		static bool axis_overlaps(const Polygon& a, const Polygon& b, const Vec2F& axis, float* amount);

		void project(const Vec2F& axis, float* min, float* max) const;
		void foreach_point(const std::function<void(int, Vec2F&, Vec2F&)>& fn);
	};
}

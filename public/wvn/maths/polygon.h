#pragma once

#include <wvn/container/vector.h>
#include <wvn/container/function.h>
#include <wvn/maths/vec2.h>
#include <wvn/maths/vec3.h>

namespace wvn
{
	/*
	 * Representation of a 2d polygon as a list of vertices.
	 */
	struct Polygon2D
	{
		Vector<Vec2F> vertices;
		
		Polygon2D();
		Polygon2D(const Vector<Vec2F>& vs);

		static bool axis_overlaps(const Polygon2D& a, const Polygon2D& b, const Vec2F& axis, float* amount);

		void project(const Vec2F& axis, float* min, float* max) const;
		void foreach_point(const Function<void(int, Vec2F&, Vec2F&)>& fn);
	};

	/*
	 * Representation of a 3d polygon as a list of vertices.
	 */
	struct Polygon3D
	{
		Vector<Vec3F> vertices;

		Polygon3D();
		Polygon3D(const Vector<Vec3F>& vs);

		static bool axis_overlaps(const Polygon3D& a, const Polygon3D& b, const Vec3F& axis, float* amount);

		void project(const Vec3F& axis, float* min, float* max) const;
		void foreach_point(const Function<void(int, Vec3F&, Vec3F&)>& fn);
	};
}

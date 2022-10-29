#include <wvn/maths/polygon.h>
#include <wvn/maths/calc.h>
#include <wvn/maths/vec2.h>

using namespace wvn;

Polygon2D::Polygon2D()
: vertices()
{
}

Polygon2D::Polygon2D(const Vector<Vec2F>& vs)
	: vertices(vs)
{
}

bool Polygon2D::axis_overlaps(const Polygon2D& a, const Polygon2D& b, const Vec2F& axis, float* amount)
{
	float min_a = 0.0f, max_a = 0.0f;
	float min_b = 0.0f, max_b = 0.0f;

	a.project(axis, &min_a, &max_a);
	b.project(axis, &min_b, &max_b);

	if (CalcF::abs(min_b - max_a) < CalcF::abs(max_b - min_a))
		(*amount) = min_b - max_a;
	else
		(*amount) = max_b - min_a;

	return (
		(min_a < max_b) &&
		(max_a > min_b)
	);
}

void Polygon2D::project(const Vec2F& axis, float* min, float* max) const
{
	float dot = Vec2F::dot(vertices[0], axis);
	(*min) = dot;
	(*max) = dot;

	for (int i = 1; i < vertices.size(); i++)
	{
		dot = Vec2F::dot(vertices[i], axis);
		(*min) = CalcF::min(dot, *min);
		(*max) = CalcF::max(dot, *max);
	}
}

void Polygon2D::foreach_point(const Function<void(int, Vec2F&, Vec2F&)>& fn)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		Vec2F curr = vertices[i];
		Vec2F next = vertices[((i+1) >= vertices.size()) ? 0 : (i+1)];

		fn.call(i, curr, next);
	}
}

/***************************************/

Polygon3D::Polygon3D()
	: vertices()
{
}

Polygon3D::Polygon3D(const Vector<Vec3F>& vs)
	: vertices(vs)
{
}

bool Polygon3D::axis_overlaps(const Polygon3D& a, const Polygon3D& b, const Vec3F& axis, float* amount)
{
	float min_a = 0.0f, max_a = 0.0f;
	float min_b = 0.0f, max_b = 0.0f;

	a.project(axis, &min_a, &max_a);
	b.project(axis, &min_b, &max_b);

	if (CalcF::abs(min_b - max_a) < CalcF::abs(max_b - min_a))
		(*amount) = min_b - max_a;
	else
		(*amount) = max_b - min_a;

	return (
		(min_a < max_b) &&
		(max_a > min_b)
	);
}

void Polygon3D::project(const Vec3F& axis, float* min, float* max) const
{
	float dot = Vec3F::dot(vertices[0], axis);
	(*min) = dot;
	(*max) = dot;

	for (int i = 1; i < vertices.size(); i++)
	{
		dot = Vec3F::dot(vertices[i], axis);
		(*min) = CalcF::min(dot, *min);
		(*max) = CalcF::max(dot, *max);
	}
}

void Polygon3D::foreach_point(const Function<void(int, Vec3F&, Vec3F&)>& fn)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		Vec3F curr = vertices[i];
		Vec3F next = vertices[((i+1) >= vertices.size()) ? 0 : (i+1)];

		fn.call(i, curr, next);
	}
}

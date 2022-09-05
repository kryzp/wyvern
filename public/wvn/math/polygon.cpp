#include <wvn/math/polygon.h>
#include <wvn/math/calc.h>
#include <wvn/math/vec2.h>

using namespace wvn;

Polygon::Polygon()
{
}

Polygon::Polygon(const Vector<Vec2F>& vs)
	: vertices(vs)
{
}

bool Polygon::axis_overlaps(const Polygon& a, const Polygon& b, const Vec2F& axis, float* amount)
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

void Polygon::project(const Vec2F& axis, float* min, float* max) const
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

void Polygon::foreach_point(const std::function<void(int, Vec2F&, Vec2F&)>& fn)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		Vec2F curr = vertices[i];
		Vec2F next = vertices[((i+1) >= vertices.size()) ? 0 : (i+1)];
		fn(i, curr, next);
	}
}

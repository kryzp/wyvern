#ifndef AGENT_SPHERE_SPHERE_H_
#define AGENT_SPHERE_SPHERE_H_

#include <wvn/physics/agent.h>
#include <wvn/physics/shapes/shape_sphere.h>

namespace wvn::phys
{
	class AgentSphereSphere : public Agent
	{
	public:
		AgentSphereSphere() = default;
		~AgentSphereSphere() override = default;

		CollisionHit test(const CollisionShape& a, const CollisionShape& b) override
		{
			const ShapeSphere& as = (const ShapeSphere&)a;
			const ShapeSphere& bs = (const ShapeSphere&)b;

			Vec3F s = as.pos - bs.pos;
			double r = as.rad + bs.rad;

			CollisionHit hit;
			hit.intersect = s.length_squared() < r*r;

			return hit;
		}
	};
}

#endif // AGENT_SPHERE_SPHERE_H_

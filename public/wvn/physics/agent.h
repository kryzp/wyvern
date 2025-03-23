#ifndef AGENT_H_
#define AGENT_H_

#include <wvn/maths/vec3.h>

namespace wvn::phys
{
	class CollisionShape;

	struct CollisionHit
	{
		bool intersect;
	};

	class Agent
	{
	public:
		Agent() = default;
		virtual ~Agent() = default;

		virtual CollisionHit test(const CollisionShape& a, const CollisionShape& b) = 0;
	};
}

#endif // AGENT_H_

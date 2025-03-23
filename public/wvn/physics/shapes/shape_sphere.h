#ifndef SHAPE_SPHERE_H
#define SHAPE_SPHERE_H

#include <wvn/physics/shape.h>

namespace wvn::phys
{
	class ShapeSphere : public CollisionShape
	{
	public:
		ShapeSphere()
			: CollisionShape(COLLISION_SHAPE_TYPE_SPHERE)
			, pos(Vec3F::zero())
			, rad(0.0f)
		{
		}

		Vec3F pos;
		float rad;
	};
}

#endif // SHAPE_SPHERE_H

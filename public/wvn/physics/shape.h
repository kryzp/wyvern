#ifndef SHAPE_H_
#define SHAPE_H_

#include <wvn/maths/vec3.h>

namespace wvn::phys
{
	enum CollisionShapeType
	{
		COLLISION_SHAPE_TYPE_NONE   = 0 << 0,
		COLLISION_SHAPE_TYPE_SPHERE = 1 << 0,
		COLLISION_SHAPE_TYPE_POLY   = 1 << 1
	};

	class CollisionShape
	{
	public:
		CollisionShape(CollisionShapeType type) : type(type) { }
		virtual ~CollisionShape() = default;

		CollisionShapeType type;
	};
}

#endif // SHAPE_H_

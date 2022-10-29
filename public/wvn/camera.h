#pragma once

#include <wvn/maths/mat4x4.h>
#include <wvn/maths/mat4x3.h>
#include <wvn/maths/vec3.h>
#include <wvn/maths/quaternion.h>

namespace wvn
{
	class Camera
	{
	public:
		Camera();
		~Camera();

		Mat4x4 view_matrix() const;

		Mat4x3 transform;
		Vec3F up;
		Quaternion direction;
	};
}

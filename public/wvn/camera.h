#ifndef CAMERA_H
#define CAMERA_H

#include <wvn/maths/mat4x4.h>
#include <wvn/maths/mat3x4.h>
#include <wvn/maths/vec3.h>
#include <wvn/maths/quat.h>
#include <wvn/maths/transform_3d.h>
#include <wvn/graphics/render_target.h>

namespace wvn
{
	// should this be moved to /graphics/ ?

	class Camera
	{
	public:
		Camera();
		~Camera();

		void render_to(const gfx::RenderTarget& target);

		Mat4x4 view_matrix() const;
		Mat4x4 view_matrix_basis() const;

		Mat4x4 proj_matrix() const;

		Transform3D transform;
		Vec3F up;
		Vec3F direction;

		float fov;
		float near;
		float far;
	};
}

#endif // CAMERA_H

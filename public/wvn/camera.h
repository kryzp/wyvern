#ifndef CAMERA_H_
#define CAMERA_H_

#include <wvn/maths/mat4x4.h>
#include <wvn/maths/affine_3d.h>
#include <wvn/maths/vec3.h>
#include <wvn/maths/quat.h>
#include <wvn/maths/transform_3d.h>
#include <wvn/graphics/render_target.h>

namespace wvn
{
	class Camera
	{
	public:
		enum CameraType
		{
			CAM_NONE,

			CAM_ORTHO,
			CAM_PERSP,

			CAM_MAX_ENUM
		};

		Camera(CameraType type, float width, float height);
		~Camera();

		void render_to(const gfx::RenderTarget& target);

		Mat4x4 view_matrix() const;
		Mat4x4 proj_matrix() const;

		CameraType type;

		Vec3F position;
		Vec3F up;
		Vec3F direction;

		float width;
		float height;

		float fov;
		float near;
		float far;
	};
}

#endif // CAMERA_H_

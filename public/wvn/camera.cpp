#include <wvn/camera.h>
#include <wvn/root.h>

using namespace wvn;

Camera::Camera(CameraType type, float width, float height)
	: type(type)
	, position(Vec3F::zero())
	, up(Vec3F::up())
	, direction(Vec3F::forward())
	, width(width)
	, height(height)
	, fov(0.0f)
	, near(0.0f)
	, far(0.0f)
{
}

Camera::~Camera()
{
}

void Camera::render_to(const gfx::RenderTarget& target)
{
	// todo
}

Mat4x4 Camera::view_matrix() const
{
	return Mat4x4::create_lookat(
		position,
		position + direction,
		up
	);
}

Mat4x4 Camera::proj_matrix() const
{
	if (type == CAM_ORTHO)
	{
		return Mat4x4::create_orthographic_ext(
			-width * 0.5f,
			width * 0.5f,
			-height * 0.5f,
			height * 0.5f,
			near,
			far
		);
	}
	else if (type == CAM_PERSP)
	{
		return Mat4x4::create_perspective(
			fov,
			width / height,
			near,
			far
		);
	}

	wvn_ERROR("[CAMERA|DEBUG] No valid camera type assigned: %d", type);
	return Mat4x4::identity();
}

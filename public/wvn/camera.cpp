#include <wvn/camera.h>
#include <wvn/root.h>

using namespace wvn;

Camera::Camera()
	: transform()
	, up(Vec3F::up())
	, direction(Vec3F::forward())
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
		transform.position(),
		transform.position() + direction,
		up
	);
}

Mat4x4 Camera::proj_matrix() const
{
	const float ASPECT = (float)Root::get_singleton()->config().width / (float)Root::get_singleton()->config().height;

	return Mat4x4::create_perspective(
		fov,
		ASPECT,
		near,
		far
	);
}

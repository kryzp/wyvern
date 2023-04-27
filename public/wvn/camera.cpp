#include <wvn/camera.h>
#include <wvn/root.h>

using namespace wvn;

Camera::Camera()
	: transform()
	, up(Vec3F::up())
	, direction(Quaternion::from_axis_angle(Vec3F::forward(), 0.0f))
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

#include <wvn/devenv/log_mgr.h>

Mat4x4 Camera::view_matrix() const
{
	dev::LogMgr::get_singleton()->print("%f %f %f %f", direction.s, direction.i, direction.j, direction.k);

	return Mat4x4::create_lookat(
		transform.position(),
		transform.position() + Quaternion::to_euler(direction),
		up
	);
}

Mat4x4 Camera::proj_matrix() const
{
	const float ASPECT = static_cast<float>(Root::get_singleton()->config().width) / static_cast<float>(Root::get_singleton()->config().height);

	return Mat4x4::create_perspective(
		fov,
		ASPECT,
		near,
		far
	);
}

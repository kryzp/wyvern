#include <wvn/camera.h>

using namespace wvn;

Camera::Camera()
	: transform(1.0f)
	, up(),
	, direction()
{
}

Camera::~Camera()
{
}

Mat4x4 Camera::view_matrix() const
{
	return Mat4x4::create_lookat(transform.position(), transform.position() + Quaternion::to_euler(direction), up);
}

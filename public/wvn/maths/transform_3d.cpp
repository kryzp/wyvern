#include <wvn/maths/transform_3d.h>

using namespace wvn;

Transform3D::Transform3D()
	: m_dirty(false)
	, m_matrix(1.0f)
	, m_position(0.0f, 0.0f, 0.0f)
	, m_rotation()
	, m_scale(1.0f, 1.0f, 1.0f)
	, m_origin(0.0f, 0.0f, 0.0f)
	, on_transformed(nullptr)
{
	recompute_matrix();
}

Transform3D::~Transform3D()
{
}

void Transform3D::move(const wvn::Vec3F& dv)
{
	if (dv == Vec3F::zero()) {
		return;
	}

	if (on_transformed) {
		on_transformed();
	}

	m_position += dv;
	m_dirty = true;
}

void Transform3D::move(float dx, float dy, float dz)
{
	if (dx == 0.0f && dy == 0.0f && dz == 0.0f) {
		return;
	}

	if (on_transformed) {
		on_transformed();
	}

	m_position.x += dx;
	m_position.y += dy;
	m_position.z += dz;

	m_dirty = true;
}

void Transform3D::move_x(float dx)
{
	if (dx == 0.0f) {
		return;
	}

	if (on_transformed) {
		on_transformed();
	}

	m_position.x += dx;
	m_dirty = true;
}

void Transform3D::move_y(float dy)
{
	if (dy == 0.0f) {
		return;
	}

	if (on_transformed) {
		on_transformed();
	}

	m_position.y += dy;
	m_dirty = true;
}

void Transform3D::move_z(float dz)
{
	if (dz == 0.0f) {
		return;
	}

	if (on_transformed) {
		on_transformed();
	}

	m_position.z += dz;
	m_dirty = true;
}

void Transform3D::position(const Vec3F& v)
{
	if (v == m_position) {
		return;
	}

	if (on_transformed) {
		on_transformed();
	}

	m_position = v;
	m_dirty = true;
}

void Transform3D::position(float x, float y, float z)
{
	if (x == m_position.x && y == m_position.y && z == m_position.z) {
		return;
	}

	if (on_transformed) {
		on_transformed();
	}

	m_position.x = x;
	m_position.y = y;
	m_position.z = z;

	m_dirty = true;
}

Vec3F Transform3D::position() const
{
	return m_position;
}

void Transform3D::origin(const Vec3F& v)
{
	if (v == m_origin) {
		return;
	}

	if (on_transformed) {
		on_transformed();
	}

	m_origin = v;
	m_dirty = true;
}

void Transform3D::origin(float x, float y, float z)
{
	if (x == m_origin.x && y == m_origin.y && z == m_origin.z) {
		return;
	}

	if (on_transformed) {
		on_transformed();
	}

	m_origin.x = x;
	m_origin.y = y;
	m_origin.z = z;

	m_dirty = true;
}

Vec3F Transform3D::origin() const
{
	return m_origin;
}

void Transform3D::scale(const wvn::Size3& v)
{
	if (v == m_scale) {
		return;
	}

	if (on_transformed) {
		on_transformed();
	}

	m_scale = v;
	m_dirty = true;
}

void Transform3D::scale(float scalar)
{
	if (scalar == m_scale.x && scalar == m_scale.y && scalar == m_scale.z) {
		return;
	}

	if (on_transformed) {
		on_transformed();
	}

	m_scale.x = scalar;
	m_scale.y = scalar;
	m_scale.z = scalar;

	m_dirty = true;
}

void Transform3D::scale(float x, float y, float z)
{
	if (x == m_scale.x && y == m_scale.y && z == m_scale.z) {
		return;
	}

	if (on_transformed) {
		on_transformed();
	}

	m_scale.x = x;
	m_scale.y = y;
	m_scale.z = z;

	m_dirty = true;
}

void Transform3D::scale_by(const Size3& dv)
{
	if (dv == Size3::zero()) {
		return;
	}

	if (on_transformed) {
		on_transformed();
	}

	m_scale += dv;
	m_dirty = true;
}

void Transform3D::scale_by(float ds)
{
	if (ds == 0.0f) {
		return;
	}

	if (on_transformed) {
		on_transformed();
	}

	m_scale.x += ds;
	m_scale.y += ds;
	m_scale.z += ds;

	m_dirty = true;
}

void Transform3D::scale_by(float dx, float dy, float dz)
{
	if (dx == 0.0f && dy == 0.0f && dz == 0.0f) {
		return;
	}

	if (on_transformed) {
		on_transformed();
	}

	m_scale.x += dx;
	m_scale.y += dy;
	m_scale.z += dz;

	m_dirty = true;
}

Size3 Transform3D::scale() const
{
	return m_scale;
}

void Transform3D::rotate(const Vec3F& axis, float angle)
{
	if (on_transformed) {
		on_transformed();
	}

	m_rotation = m_rotation.rotate_on_axis(axis, angle);
}

void Transform3D::rotate(const Quaternion& v)
{
	if (on_transformed) {
		on_transformed();
	}

	m_rotation *= v;
}

void Transform3D::rotation(const Vec3F& axis, float angle)
{
	if (on_transformed) {
		on_transformed();
	}

	m_rotation = Quaternion::from_axis_angle(axis, angle);
}

void Transform3D::rotation(const Quaternion& v)
{
	if (on_transformed) {
		on_transformed();
	}

	m_rotation = v;
}

Quaternion Transform3D::rotation() const
{
	return m_rotation;
}

Mat4x3 Transform3D::matrix()
{
	if (m_dirty) {
		recompute_matrix();
		m_dirty = false;
	}

	return m_matrix;
}

void Transform3D::recompute_matrix()
{
	m_matrix = Mat4x3::create_transform(
		m_position,
		m_rotation,
		m_scale,
		m_origin
	);
}

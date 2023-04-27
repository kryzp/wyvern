#ifndef TRANSFORM_3D_H
#define TRANSFORM_3D_H

#include <wvn/maths/vec3.h>
#include <wvn/maths/quaternion.h>
#include <wvn/maths/mat4x3.h>
#include <wvn/container/function.h>

namespace wvn
{
	struct Transform3D
	{
	public:
		Transform3D();
		~Transform3D();

		void move(const Vec3F& dv);
		void move(float dx, float dy, float dz);
		void move_x(float dx);
		void move_y(float dy);
		void move_z(float dz);
		void position(const Vec3F& v);
		void position(float x, float y, float z);
		Vec3F position() const;

		void origin(const Vec3F& v);
		void origin(float x, float y, float z);
		Vec3F origin() const;

		void scale(const Size3& v);
		void scale(float scalar);
		void scale(float x, float y, float z);
		void scale_by(const Size3& dv);
		void scale_by(float ds);
		void scale_by(float dx, float dy, float dz);
		Size3 scale() const;

		void rotate(const Vec3F& axis, float angle);
		void rotate(const Quaternion& v);
		void rotation(const Vec3F& axis, float angle);
		void rotation(const Quaternion& v);
		Quaternion rotation() const;

		Mat4x3 matrix();

		Function<void(void)> on_transformed;

	private:
		void recompute_matrix();

		bool m_dirty;
		Mat4x3 m_matrix;

		Vec3F m_position;
		Quaternion m_rotation;
		Size3 m_scale;
		Vec3F m_origin;
	};
}

#endif // TRANSFORM_3D_H

#ifndef TRANSFORM_3D_H
#define TRANSFORM_3D_H

#include <wvn/maths/vec3.h>
#include <wvn/maths/quat.h>
#include <wvn/maths/mat3x4.h>
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

		void scale(const Vec3F& v);
		void scale(float scalar);
		void scale(float x, float y, float z);
		void scale_by(const Vec3F& dv);
		void scale_by(float ds);
		void scale_by(float dx, float dy, float dz);
		Vec3F scale() const;

		void rotate(const Vec3F& axis, float angle);
		void rotate(const Quat& quat);
		void rotation(const Vec3F& axis, float angle);
		void rotation(const Quat& quat);
		Quat rotation() const;

		Mat3x4 matrix();

		Function<void(void)> on_transformed;

	private:
		void recompute_matrix();

		bool m_dirty;
		Mat3x4 m_matrix;

		Vec3F m_position;
		Quat m_rotation;
		Vec3F m_scale;
		Vec3F m_origin;
	};
}

#endif // TRANSFORM_3D_H

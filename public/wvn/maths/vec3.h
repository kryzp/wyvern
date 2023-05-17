#ifndef VEC3_H
#define VEC3_H

#include <wvn/maths/calc.h>
#include <wvn/maths/random.h>
#include <wvn/maths/quat.h>
#include <wvn/maths/vec2.h>
#include <wvn/maths/mat3x4.h>

namespace wvn
{
	template <typename T>
	struct Vec3
	{
		union
		{
			struct
			{
				T x;
				T y;
				T z;
			};

			T data[3];
		};

		Vec3();
		Vec3(T s);
		Vec3(T x, T y, T z);
		explicit Vec3(const Vec2<T>& xy);

		// enable implicit casting to other vectors
		template <typename Y>
		Vec3(const Vec3<Y>& other) noexcept
			: x(other.x)
			, y(other.y)
			, z(other.z)
		{
		}

		static const Vec3& unit();
		static const Vec3& zero();
		static const Vec3& one();
		static const Vec3& left();
		static const Vec3& right();
		static const Vec3& up();
		static const Vec3& down();
		static const Vec3& forward();
		static const Vec3& backward();

		static Vec3 from_angle(float theta, float phi, float radius);
		static T dot(const Vec3& a, const Vec3& b);
		static Vec3 cross(const Vec3& a, const Vec3& b);
		static Vec3 transform(const Vec3& vec, const Mat3x4& mat);
		static Vec3 lerp(const Vec3& from, const Vec3& to, float amount);
		static Vec3 spring(const Vec3& from, const Vec3& to, float bounciness, float tension, Vec3& intermediate);
		static Vec3 approach(const Vec3& from, const Vec3& to, float amount);

		static Vec3 reflect(const Vec3& v, const Vec3& n);
		static Vec3 refract(const Vec3& uv, const Vec3& n, double n21);

		Vec3<T> rotate(float angle, const Vec3<T>& axis);
		Vec3<T> rotate(const Quat& quat);

		T length() const;
		T length_squared() const;

		Vec3 abs() const;
		Vec3 normalized() const;
		Vec2<T> xy() const;

		bool operator == (const Vec3& other) const;
		bool operator != (const Vec3& other) const;

		Vec3<T> operator + (const Vec3& other) const;
		Vec3<T> operator - (const Vec3& other) const;
		Vec3<T> operator * (const Vec3& other) const;
		Vec3<T> operator / (const Vec3& other) const;

		Vec3<T> operator - () const;

		Vec3<T>& operator += (const Vec3& other);
		Vec3<T>& operator -= (const Vec3& other);
		Vec3<T>& operator *= (const Vec3& other);
		Vec3<T>& operator /= (const Vec3& other);
	};

	// global operators
	template <typename T> Vec3<T> operator * (const Vec3<T>& lhs, float rhs) { return Vec3<T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs); }
	template <typename T> Vec3<T> operator * (float lhs, const Vec3<T>& rhs) { return Vec3<T>(rhs.x * lhs, rhs.y * lhs, rhs.z * lhs); }

	using Vec3F		= Vec3<float>;
	using Vec3D		= Vec3<double>;
	using Vec3I		= Vec3<int>;
	using Vec3U		= Vec3<unsigned>;

	using Float3	= Vec3<float>;
	using Double3	= Vec3<double>;
	using Size3		= Vec3<float>;
	using Int3		= Vec3<int>;
	using Point3	= Vec3<float>;
	using UInt3		= Vec3<unsigned>;
	using Unsigned3 = Vec3<unsigned>;
	
	template <typename T>
	Vec3<T>::Vec3()
		: x(0.0), y(0.0), z(0.0)
	{
	}

	template <typename T>
	Vec3<T>::Vec3(T s)
		: x(s), y(s), z(s)
	{
	}

	template <typename T>
	Vec3<T>::Vec3(T x, T y, T z)
		: x(x), y(y), z(z)
	{
	}

	template <typename T>
	Vec3<T>::Vec3(const Vec2<T>& xy)
		: x(xy.x), y(xy.y), z(0.0f)
	{
	}

	template <typename T>
	Vec3<T> Vec3<T>::from_angle(float theta, float phi, float radius)
	{
		return Vec3(
			radius * Calc<T>::cos(theta) * Calc<T>::cos(phi),
			radius * Calc<T>::sin(theta),
			radius * Calc<T>::cos(theta) * Calc<T>::sin(phi)
		);
	}

	template <typename T>
	T Vec3<T>::dot(const Vec3<T>& a, const Vec3<T>& b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	}

	template <typename T>
	Vec3<T> Vec3<T>::cross(const Vec3<T>& a, const Vec3<T>& b)
	{
		return Vec3(
			(a.y * b.z) - (a.z * b.y),
			(a.z * b.x) - (a.x * b.z),
			(a.x * b.y) - (a.y * b.x)
		);
	}

	template <typename T>
	Vec3<T> Vec3<T>::transform(const Vec3<T>& vec, const Mat3x4& mat)
	{
		return Vec3(
			(vec.x * mat.m11) + (vec.y * mat.m21) + (vec.z * mat.m31) + mat.m14,
			(vec.x * mat.m12) + (vec.y * mat.m22) + (vec.z * mat.m32) + mat.m24,
			(vec.x * mat.m13) + (vec.y * mat.m23) + (vec.z * mat.m33) + mat.m34
		);
	}
	
	template <typename T>
	Vec3<T> Vec3<T>::lerp(const Vec3& from, const Vec3& to, float amount)
	{
		return Vec3(
			Calc<T>::lerp(from.x, to.x, amount),
			Calc<T>::lerp(from.y, to.y, amount),
			Calc<T>::lerp(from.z, to.z, amount)
		);
	}

	template <typename T>
	Vec3<T> Vec3<T>::spring(const Vec3& from, const Vec3& to, float bounciness, float tension, Vec3& intermediate)
	{
		return Vec3(
			Calc<T>::spring(from.x, to.x, bounciness, tension, intermediate.x),
			Calc<T>::spring(from.y, to.y, bounciness, tension, intermediate.y),
			Calc<T>::spring(from.z, to.z, bounciness, tension, intermediate.z)
		);
	}

	template <typename T>
	Vec3<T> Vec3<T>::approach(const Vec3& from, const Vec3& to, float amount)
	{
		return Vec3(
			Calc<T>::approach(from.x, to.x, amount),
			Calc<T>::approach(from.y, to.y, amount),
			Calc<T>::approach(from.z, to.z, amount)
		);
	}

	template <typename T>
	Vec3<T> Vec3<T>::reflect(const Vec3<T>& v, const Vec3<T>& n)
	{
		return v - (2.0 * dot(v, n) * n);
	}

	template <typename T>
	Vec3<T> Vec3<T>::refract(const Vec3<T>& uv, const Vec3<T>& n, double n21)
	{
		double cost = Calc<T>::min(dot(-uv, n), 1.0);
		Vec3 out_perp = n21 * (uv + (cost * n));
		Vec3 out_para = -Calc<T>::sqrt(Calc<T>::abs(1.0 - out_perp.length_squared())) * n;
		return out_perp + out_para;
	}

	template <typename T>
	Vec3<T> Vec3<T>::rotate(float angle, const Vec3<T>& axis)
	{
		return Vec3<T>::transform(*this, Mat3x4::create_rotation(axis, angle));
		/*
		T cost = Calc<T>::cos(angle);
		T sint = Calc<T>::sin(angle);
		T icost = 1.0 - cost;

		return Vec3(
			(this->x * ((axis.x * axis.x * icost) +           cost))  + (this->y * ((axis.x * axis.y * icost) - (axis.z * sint))) + (this->z * ((axis.x * axis.z * icost) + (axis.y * sint))),
			(this->x * ((axis.y * axis.x * icost) + (axis.z * sint))) + (this->y * ((axis.y * axis.y * icost) +           cost))  + (this->z * ((axis.y * axis.z * icost) - (axis.x * sint))),
			(this->x * ((axis.z * axis.x * icost) - (axis.y * sint))) + (this->y * ((axis.z * axis.y * icost) + (axis.x * sint))) + (this->z * ((axis.z * axis.z * icost) +  cost          ))
		);
		*/
	}

	template <typename T>
	Vec3<T> Vec3<T>::rotate(const Quat& quat)
	{
		return (quat * Quat(x, y, z) * quat.inverse()).vector();
	}

	template <typename T>
	T Vec3<T>::length() const
	{
		return Calc<T>::sqrt(length_squared());
	}

	template <typename T>
	T Vec3<T>::length_squared() const
	{
		return (x * x) + (y * y) + (z * z);
	}

	template <typename T>
	Vec3<T> Vec3<T>::abs() const
	{
		return Vec3(
			Calc<T>::abs(this->x),
			Calc<T>::abs(this->y),
			Calc<T>::abs(this->z)
		);
	}

	template <typename T>
	Vec3<T> Vec3<T>::normalized() const
	{
		float len = length();

		if (len <= 0.0) {
			return zero();
		}

		return Vec3(
			this->x / len,
			this->y / len,
			this->z / len
		);
	}

	template <typename T>
	Vec2<T> Vec3<T>::xy() const
	{
		return Vec2<T>(
			this->x,
			this->y
		);
	}

	template <typename T> bool Vec3<T>::operator == (const Vec3& other) const { return this->x == other.x && this->y == other.y && this->z == other.z; }
	template <typename T> bool Vec3<T>::operator != (const Vec3& other) const { return !(*this == other); }

	template <typename T> Vec3<T> Vec3<T>::operator + (const Vec3& other) const { return Vec3(this->x + other.x, this->y + other.y, this->z + other.z); }
	template <typename T> Vec3<T> Vec3<T>::operator - (const Vec3& other) const { return Vec3(this->x - other.x, this->y - other.y, this->z - other.z); }
	template <typename T> Vec3<T> Vec3<T>::operator * (const Vec3& other) const { return Vec3(this->x * other.x, this->y * other.y, this->z * other.z); }
	template <typename T> Vec3<T> Vec3<T>::operator / (const Vec3& other) const { return Vec3(this->x / other.x, this->y / other.y, this->z / other.z); }

	template <typename T> Vec3<T> Vec3<T>::operator - () const { return Vec3(-this->x, -this->y, -this->z); }

	template <typename T> Vec3<T>& Vec3<T>::operator += (const Vec3& other) { this->x += other.x; this->y += other.y; this->z += other.z; return *this; }
	template <typename T> Vec3<T>& Vec3<T>::operator -= (const Vec3& other) { this->x -= other.x; this->y -= other.y; this->z -= other.z; return *this; }
	template <typename T> Vec3<T>& Vec3<T>::operator *= (const Vec3& other) { this->x *= other.x; this->y *= other.y; this->z *= other.z; return *this; }
	template <typename T> Vec3<T>& Vec3<T>::operator /= (const Vec3& other) { this->x /= other.x; this->y /= other.y; this->z /= other.z; return *this; }

	template <typename T> const Vec3<T>& Vec3<T>::unit()		{ static const Vec3 UNIT		= Vec3( 1,  1,  1); return UNIT;		}
	template <typename T> const Vec3<T>& Vec3<T>::zero()		{ static const Vec3 ZERO		= Vec3( 0,  0,  0); return ZERO;		}
	template <typename T> const Vec3<T>& Vec3<T>::one()			{ static const Vec3 ONE			= Vec3( 1,  1,  1); return ONE;			}
	template <typename T> const Vec3<T>& Vec3<T>::left()		{ static const Vec3 LEFT		= Vec3(-1,  0,  0); return LEFT;		}
	template <typename T> const Vec3<T>& Vec3<T>::right()		{ static const Vec3 RIGHT		= Vec3( 1,  0,  0); return RIGHT;		}
	template <typename T> const Vec3<T>& Vec3<T>::up()			{ static const Vec3 UP			= Vec3( 0,  1,  0); return UP;			}
	template <typename T> const Vec3<T>& Vec3<T>::down()		{ static const Vec3 DOWN		= Vec3( 0, -1,  0); return DOWN;		}
	template <typename T> const Vec3<T>& Vec3<T>::forward()		{ static const Vec3 FORWARD		= Vec3( 0,  0,  1); return FORWARD;		}
	template <typename T> const Vec3<T>& Vec3<T>::backward()	{ static const Vec3 BACKWARD	= Vec3( 0,  0, -1); return BACKWARD;	}
}

#endif // VEC3_H

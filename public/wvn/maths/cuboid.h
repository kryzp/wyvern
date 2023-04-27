#ifndef CUBOID_H
#define CUBOID_H

#include <wvn/maths/vec3.h>
#include <wvn/maths/rect.h>

namespace wvn
{
	/**
	 * Representation of a cuboid.
	 */
	template <typename T>
	struct Cuboid
	{
		T x, y, z;
		T w, h, l;

		Cuboid();
		Cuboid(T v);
		Cuboid(T w, T h, T l);
		Cuboid(T x, T y, T z, T w, T h, T l);

		// enable implicit casting to other cuboids
		template <typename Y>
		Cuboid(const Cuboid<Y>& other) noexcept
			: x(other.x)
			, y(other.y)
			, z(other.z)
			, w(other.w)
			, h(other.h)
			, l(other.l)
		{
		}

		static const Cuboid& zero();
		static const Cuboid& unit();

		bool contains(const Vec3<T>& other) const;
		bool intersects(const Cuboid& other) const;

		Vec3<T> position() const;
		Vec3<T> size() const;

		Rect<T> front_face() const;
		Rect<T> bottom_face() const;
		Rect<T> side_face() const;

		T left() const;
		T right() const;
		T top() const;
		T bottom() const;
		T front() const;
		T back() const;

		bool operator == (const Cuboid& other) const;
		bool operator != (const Cuboid& other) const;

		Cuboid operator + (const Cuboid& other) const;
		Cuboid operator - (const Cuboid& other) const;
		Cuboid operator * (const Cuboid& other) const;
		Cuboid operator / (const Cuboid& other) const;

		Cuboid operator - () const;

		Cuboid& operator += (const Cuboid& other);
		Cuboid& operator -= (const Cuboid& other);
		Cuboid& operator *= (const Cuboid& other);
		Cuboid& operator /= (const Cuboid& other);
	};

	using CuboidI = Cuboid<int>;
	using CuboidF = Cuboid<float>;

	using Rect3DI = Cuboid<int>;
	using Rect3DF = Cuboid<float>;

	template <typename T>
	Cuboid<T>::Cuboid()
		: x(0), y(0), z(0)
		, w(0), h(0), l(0)
	{
	}

	template <typename T>
	Cuboid<T>::Cuboid(T v)
		: x(0), y(0), z(0)
		, w(v), h(v), l(v)
	{
	}

	template <typename T>
	Cuboid<T>::Cuboid(T w, T h, T l)
		: x(0), y(0), z(0)
		, w(w), h(h), l(l)
	{
	}

	template <typename T>
	Cuboid<T>::Cuboid(T x, T y, T z, T w, T h, T l)
		: x(x), y(y), z(z)
		, w(w), h(h), l(l)
	{
	}

	template <typename T>
	bool Cuboid<T>::contains(const Vec3<T>& other) const
	{
		return (

			// X
			this->left() < other.x &&
			this->right() > other.x &&

			// Y
			this->top() < other.y &&
			this->bottom() > other.y &&

			// Z
			this->front() < other.z &&
			this->back() > other.z
		);
	}

	template <typename T>
	bool Cuboid<T>::intersects(const Cuboid& other) const
	{
		return (

			// X
			this->left() < other.right() &&
			this->right() > other.left() &&

			// Y
			this->top() < other.bottom() &&
			this->bottom() > other.top() &&

			// Z
			this->front() < other.back() &&
			this->back() > other.front()
		);
	}

	template <typename T>
	Vec3<T> Cuboid<T>::position() const
	{
		return Vec3<T>(x, y, z);
	}

	template <typename T>
	Vec3<T> Cuboid<T>::size() const
	{
		return Vec3<T>(w, h, l);
	}

	template <typename T>
	Rect<T> Cuboid<T>::front_face() const
	{
		return Rect<T>(x, y, w, h);
	}

	template <typename T>
	Rect<T> Cuboid<T>::bottom_face() const
	{
		return Rect<T>(x, z, w, l);
	}

	template <typename T>
	Rect<T> Cuboid<T>::side_face() const
	{
		return Rect<T>(z, y, l, h);
	}

	template <typename T>
	T Cuboid<T>::left() const
	{
		return x;
	}

	template <typename T>
	T Cuboid<T>::right() const
	{
		return x + w;
	}

	template <typename T>
	T Cuboid<T>::top() const
	{
		return y;
	}

	template <typename T>
	T Cuboid<T>::bottom() const
	{
		return y + h;
	}

	template <typename T>
	T Cuboid<T>::front() const
	{
		return z;
	}

	template <typename T>
	T Cuboid<T>::back() const
	{
		return z + l;
	}

	template <typename T>
	bool Cuboid<T>::operator == (const Cuboid& other) const
	{
		return (
			this->x == other.x &&
			this->y == other.y &&
			this->z == other.z &&
			this->w == other.w &&
			this->h == other.h &&
			this->l == other.l
		);
	}

	template <typename T>
	bool Cuboid<T>::operator != (const Cuboid& other) const
	{
		return !(*this == other);
	}

	template <typename T>
	Cuboid<T> Cuboid<T>::operator + (const Cuboid& other) const
	{
		return Cuboid<T>(
			this->x + other.x,
			this->y + other.y,
			this->z + other.z,
			this->w + other.w,
			this->h + other.h,
			this->l + other.l
		);
	}

	template <typename T>
	Cuboid<T> Cuboid<T>::operator - (const Cuboid& other) const
	{
		return Cuboid<T>(
			this->x - other.x,
			this->y - other.y,
			this->z - other.z,
			this->w - other.w,
			this->h - other.h,
			this->l - other.l
		);
	}

	template <typename T>
	Cuboid<T> Cuboid<T>::operator * (const Cuboid& other) const
	{
		return Cuboid<T>(
			this->x * other.x,
			this->y * other.y,
			this->z * other.z,
			this->w * other.w,
			this->h * other.h,
			this->l * other.l
		);
	}

	template <typename T>
	Cuboid<T> Cuboid<T>::operator / (const Cuboid& other) const
	{
		return Cuboid<T>(
			this->x / other.x,
			this->y / other.y,
			this->z / other.z,
			this->w / other.w,
			this->h / other.h,
			this->l / other.l
		);
	}

	template <typename T>
	Cuboid<T> Cuboid<T>::operator - () const
	{
		return Cuboid<T>(
			-this->x,
			-this->y,
			-this->z,
			-this->w,
			-this->h,
			-this->l
		);
	}

	template <typename T>
	Cuboid<T>& Cuboid<T>::operator += (const Cuboid& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		this->w += other.w;
		this->h += other.h;
		this->l += other.l;

		return *this;
	}

	template <typename T>
	Cuboid<T>& Cuboid<T>::operator -= (const Cuboid& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		this->w -= other.w;
		this->h -= other.h;
		this->l -= other.l;

		return *this;
	}

	template <typename T>
	Cuboid<T>& Cuboid<T>::operator *= (const Cuboid& other)
	{
		this->x *= other.x;
		this->y *= other.y;
		this->z *= other.z;
		this->w *= other.w;
		this->h *= other.h;
		this->l *= other.l;

		return *this;
	}

	template <typename T>
	Cuboid<T>& Cuboid<T>::operator /= (const Cuboid& other)
	{
		this->x /= other.x;
		this->y /= other.y;
		this->z /= other.z;
		this->w /= other.w;
		this->h /= other.h;
		this->l /= other.l;

		return *this;
	}

	template <typename T>
	const Cuboid<T>& Cuboid<T>::zero()
	{
		static const Cuboid ZERO = Cuboid(0, 0, 0, 0, 0, 0);
		return ZERO;
	}

	template <typename T>
	const Cuboid<T>& Cuboid<T>::unit()
	{
		static const Cuboid UNIT = Cuboid(0, 0, 0, 1, 1, 1);
		return UNIT;
	}
}

#endif // CUBOID_H

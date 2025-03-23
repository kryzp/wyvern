#ifndef ARRAY_H_
#define ARRAY_H_

#include <initializer_list>
#include <wvn/common.h>

namespace wvn
{
	/**
	 * Wrapper around a c-style array that provides some
	 * utility functions and makes them easier to work with.
	 */
	template <typename T, u64 Size>
	class Array
	{
	public:
		Array();
		Array(std::initializer_list<T> data);
		Array(const Array& other);
		Array& operator = (const Array& other);
		~Array();

		void fill(const T& value);
		void clear();

		constexpr u64 memory_size() const;
		constexpr u64 size() const;

		T* data();
		const T* data() const;

		T* begin();
		const T* begin() const;
		T* end();
		const T* end() const;

		const T* cbegin() const;
		const T* cend() const;

		T& at(u64 idx);
		const T& at(u64 idx) const;

		T& operator [] (u64 idx);
		const T& operator [] (u64 idx) const;

	private:
		T m_buf[Size];
	};

	template <typename T, u64 Size>
	Array<T, Size>::Array()
		: m_buf{}
	{
	}
	
	template <typename T, u64 Size>
	Array<T, Size>::Array(std::initializer_list<T> data)
	{
		for (u64 i = 0; i < Size; i++) {
			m_buf[i] = data.begin()[i];
		}
	}
	
	template <typename T, u64 Size>
	Array<T, Size>::Array(const Array& other)
	{
		for (u64 i = 0; i < Size; i++) {
			m_buf[i] = other.m_buf[i];
		}
	}
	
	template <typename T, u64 Size>
	Array<T, Size>& Array<T, Size>::operator = (const Array& other)
	{
		for (u64 i = 0; i < Size; i++) {
			m_buf[i] = other.m_buf[i];
		}

		return *this;
	}
	
	template <typename T, u64 Size>
	Array<T, Size>::~Array()
	{
	}

	template <typename T, u64 Size>
	void Array<T, Size>::fill(const T& value)
	{
		for (int i = 0; i < Size; i++) {
			m_buf[i] = value;
		}
	}

	template <typename T, u64 Size>
	void Array<T, Size>::clear()
	{
		mem::set(m_buf, 0, memory_size());
	}

	template <typename T, u64 Size>
	constexpr u64 Array<T, Size>::memory_size() const
	{
		return sizeof(T) * Size;
	}

	template <typename T, u64 Size>
	constexpr u64 Array<T, Size>::size() const
	{
		return Size;
	}

	template <typename T, u64 Size>
	T* Array<T, Size>::data()
	{
		return m_buf;
	}

	template <typename T, u64 Size>
	const T* Array<T, Size>::data() const
	{
		return m_buf;
	}

	template <typename T, u64 Size>
	T* Array<T, Size>::begin()
	{
		return m_buf;
	}

	template <typename T, u64 Size>
	const T* Array<T, Size>::begin() const
	{
		return m_buf;
	}

	template <typename T, u64 Size>
	T* Array<T, Size>::end()
	{
		return m_buf + Size;
	}

	template <typename T, u64 Size>
	const T* Array<T, Size>::end() const
	{
		return m_buf + Size;
	}

	template <typename T, u64 Size>
	const T* Array<T, Size>::cbegin() const
	{
		return m_buf;
	}

	template <typename T, u64 Size>
	const T* Array<T, Size>::cend() const
	{
		return m_buf + Size;
	}

	template <typename T, u64 Size>
	T& Array<T, Size>::at(u64 idx)
	{
		wvn_ASSERT(idx >= 0 && idx < Size, "[ARRAY|DEBUG] Index must be within bounds: %d", idx);
		return m_buf[idx];
	}

	template <typename T, u64 Size>
	const T& Array<T, Size>::at(u64 idx) const
	{
		wvn_ASSERT(idx >= 0 && idx < Size, "[ARRAY|DEBUG] Index must be within bounds: %llu", idx);
		return m_buf[idx];
	}
	
	template <typename T, u64 Size>
	T& Array<T, Size>::operator [] (u64 idx)
	{
		wvn_ASSERT(idx >= 0 && idx < Size, "[ARRAY|DEBUG] Index must be within bounds: %llu", idx);
		return m_buf[idx];
	}
	
	template <typename T, u64 Size>
	const T& Array<T, Size>::operator [] (u64 idx) const
	{
		wvn_ASSERT(idx >= 0 && idx < Size, "[ARRAY|DEBUG] Index must be within bounds: %llu", idx);
		return m_buf[idx];
	}
}

#endif // ARRAY_H_

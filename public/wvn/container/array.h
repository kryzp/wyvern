#ifndef ARRAY_H
#define ARRAY_H

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

	template <typename T, u64 TSize>
	Array<T, TSize>::Array()
		: m_buf{}
	{
	}
	
	template <typename T, u64 TSize>
	Array<T, TSize>::Array(std::initializer_list<T> data)
	{
		for (u64 i = 0; i < TSize; i++) {
			m_buf[i] = data.begin()[i];
		}
	}
	
	template <typename T, u64 TSize>
	Array<T, TSize>::Array(const Array& other)
	{
		for (u64 i = 0; i < TSize; i++) {
			m_buf[i] = other.m_buf[i];
		}
	}
	
	template <typename T, u64 TSize>
	Array<T, TSize>& Array<T, TSize>::operator = (const Array& other)
	{
		for (u64 i = 0; i < TSize; i++) {
			m_buf[i] = other.m_buf[i];
		}
	}
	
	template <typename T, u64 TSize>
	Array<T, TSize>::~Array()
	{
	}

	template <typename T, u64 TSize>
	void Array<T, TSize>::fill(const T& value)
	{
		for (int i = 0; i < TSize; i++) {
			m_buf[i] = value;
		}
	}

	template <typename T, u64 TSize>
	constexpr u64 Array<T, TSize>::memory_size() const
	{
		return sizeof(T) * TSize;
	}

	template <typename T, u64 TSize>
	constexpr u64 Array<T, TSize>::size() const
	{
		return TSize;
	}

	template <typename T, u64 TSize>
	T* Array<T, TSize>::data()
	{
		return m_buf;
	}

	template <typename T, u64 TSize>
	const T* Array<T, TSize>::data() const
	{
		return m_buf;
	}

	template <typename T, u64 TSize>
	T* Array<T, TSize>::begin()
	{
		return &m_buf[0];
	}

	template <typename T, u64 TSize>
	const T* Array<T, TSize>::begin() const
	{
		return &m_buf[0];
	}

	template <typename T, u64 TSize>
	T* Array<T, TSize>::end()
	{
		return &m_buf[TSize];
	}

	template <typename T, u64 TSize>
	const T* Array<T, TSize>::end() const
	{
		return &m_buf[TSize];
	}

	template <typename T, u64 TSize>
	const T* Array<T, TSize>::cbegin() const
	{
		return &m_buf[0];
	}

	template <typename T, u64 TSize>
	const T* Array<T, TSize>::cend() const
	{
		return &m_buf[TSize];
	}

	template <typename T, u64 TSize>
	T& Array<T, TSize>::at(u64 idx)
	{
		WVN_ASSERT(idx >= 0 && idx < TSize, "[DEBUG|ARRAY] Index must be within bounds.");
		return m_buf[idx];
	}

	template <typename T, u64 TSize>
	const T& Array<T, TSize>::at(u64 idx) const
	{
		WVN_ASSERT(idx >= 0 && idx < TSize, "[DEBUG|ARRAY] Index must be within bounds.");
		return m_buf[idx];
	}
	
	template <typename T, u64 TSize>
	T& Array<T, TSize>::operator [] (u64 idx)
	{
		WVN_ASSERT(idx >= 0 && idx < TSize, "[DEBUG|ARRAY] Index must be within bounds.");
		return m_buf[idx];
	}
	
	template <typename T, u64 TSize>
	const T& Array<T, TSize>::operator [] (u64 idx) const
	{
		WVN_ASSERT(idx >= 0 && idx < TSize, "[DEBUG|ARRAY] Index must be within bounds.");
		return m_buf[idx];
	}
}

#endif // ARRAY_H

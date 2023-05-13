#ifndef STACK_VECTOR_H
#define STACK_VECTOR_H

#include <initializer_list>

#include <wvn/util/common.h>
#include <wvn/util/assert.h>

namespace wvn
{
	/**
	 * Stack-based vector.
	 * Basically a regular fixed-size array that keeps
	 * track of how many items are used and how many are free.
	 */
	template <typename T, u64 Capacity>
	class StackVector
	{
	public:
		StackVector();

		StackVector(std::initializer_list<T> data);

		StackVector(const StackVector& other);
		StackVector(StackVector&& other) noexcept;

		StackVector& operator = (const StackVector& other);
		StackVector& operator = (StackVector&& other) noexcept;

		~StackVector();

		void erase(u64 index, u64 amount);

		T* data();
		const T* data() const;

		T& front();
		const T& front() const;
		T& back();
		const T& back() const;

		T& push_front(const T& item);
		T& push_back(const T& item);

		T pop_front();
		T pop_back();

		template <typename... Args>
		T& emplace_front(Args&&... args);

		template <typename... Args>
		T& emplace_back(Args&&... args);

		void clear();
		u64 size() const;
		constexpr u64 capacity() const;

		bool any() const;
		bool empty() const;

		T& at(u64 idx);
		const T& at(u64 idx) const;

		T& operator [] (u64 idx);
		const T& operator [] (u64 idx) const;

	private:
		T m_buf[Capacity];
		u64 m_size;
	};

	template <typename T, u64 Capacity>
	StackVector<T, Capacity>::StackVector()
		: m_buf{}
		, m_size(0)
	{
	}

	template <typename T, u64 Capacity>
	StackVector<T, Capacity>::StackVector(std::initializer_list<T> data)
		: StackVector()
	{
		m_size = data.size();

		for (u64 i = 0; i < m_size; i++) {
			m_buf[i] = data.begin()[i];
		}
	}

	template <typename T, u64 Capacity>
	StackVector<T, Capacity>::StackVector(const StackVector& other)
		: StackVector()
	{
		if (other.m_size > 0)
		{
			clear();
			m_size = other.m_size;

			for (u64 i = 0; i < other.m_size; i++) {
				m_buf[i] = other.m_buf[i];
			}
		}
	}

	template <typename T, u64 Capacity>
	StackVector<T, Capacity>::StackVector(StackVector&& other) noexcept
		: StackVector()
	{
		this->m_size = std::move(other.m_size);
		this->m_buf = std::move(other.m_buf);

		other.m_size = 0;
		other.m_buf = nullptr;
	}

	template <typename T, u64 Capacity>
	StackVector<T, Capacity>& StackVector<T, Capacity>::operator = (const StackVector& other)
	{
		clear();

		m_size = other.m_size;

		for (u64 i = 0; i < other.m_size; i++) {
			m_buf[i] = other.m_buf[i];
		}

		return *this;
	}

	template <typename T, u64 Capacity>
	StackVector<T, Capacity>& StackVector<T, Capacity>::operator = (StackVector&& other) noexcept
	{
		clear();

		this->m_size = std::move(other.m_size);
		this->m_buf = std::move(other.m_buf);

		other.m_size = 0;
		other.m_buf = nullptr;

		return *this;
	}

	template <typename T, u64 Capacity>
	StackVector<T, Capacity>::~StackVector()
	{
		clear();

		mem::set(m_buf, 0, Capacity * sizeof(T));
		m_size = 0;
	}

	template <typename T, u64 Capacity>
	void StackVector<T, Capacity>::erase(u64 index, u64 amount)
	{
		if (amount <= 0) {
			return;
		}

		for (u64 i = 0; i < amount; i++) {
			m_buf[i + index].~T();
		}

		for (u64 i = 0; i < m_size - amount - 1; i++) {
			m_buf[i + index] = std::move(m_buf[i + index + amount]);
		}

		m_size -= amount;
	}

	template <typename T, u64 Capacity>
	T& StackVector<T, Capacity>::push_front(const T& item)
	{
		mem::move(m_buf + 1, m_buf, sizeof(T) * m_size);
		m_buf[0] = item;
		m_size++;
		return m_buf[0];
	}

	template <typename T, u64 Capacity>
	T& StackVector<T, Capacity>::push_back(const T& item)
	{
		m_buf[m_size] = std::move(item);
		m_size++;
		return m_buf[m_size - 1];
	}

	template <typename T, u64 Capacity>
	T StackVector<T, Capacity>::pop_front()
	{
		T item = std::move(m_buf[0]);
		m_buf[0].~T();
		mem::move(m_buf, m_buf + 1, m_size - 1);
		m_size--;
		return item;
	}

	template <typename T, u64 Capacity>
	T StackVector<T, Capacity>::pop_back()
	{
		T item = std::move(m_buf[m_size - 1]);
		m_buf[m_size - 1].~T();
		m_size--;
		return item;
	}

	template <typename T, u64 Capacity>
	template <typename... Args>
	T& StackVector<T, Capacity>::emplace_front(Args&&... args)
	{
		new (m_buf) T(std::forward<Args>(args)...);
		return m_buf[0];
	}

	template <typename T, u64 Capacity>
	template <typename... Args>
	T& StackVector<T, Capacity>::emplace_back(Args&&... args)
	{
		new (m_buf + m_size - 1) T(std::forward<Args>(args)...);
		return m_buf[m_size - 1];
	}

	template <typename T, u64 Capacity>
	void StackVector<T, Capacity>::clear()
	{
		for (int i = 0; i < m_size; i++) {
			m_buf[i].~T();
		}

		mem::set(m_buf, 0, m_size * sizeof(T));
		m_size = 0;
	}

	template <typename T, u64 Capacity>
	u64 StackVector<T, Capacity>::size() const
	{
		return m_size;
	}

	template <typename T, u64 Capacity>
	constexpr u64 StackVector<T, Capacity>::capacity() const
	{
		return Capacity;
	}

	template <typename T, u64 Capacity>
	bool StackVector<T, Capacity>::any() const
	{
		return m_size != 0;
	}

	template <typename T, u64 Capacity>
	bool StackVector<T, Capacity>::empty() const
	{
		return m_size == 0;
	}

	template <typename T, u64 Capacity>
	T* StackVector<T, Capacity>::data()
	{
		return m_buf;
	}

	template <typename T, u64 Capacity>
	const T* StackVector<T, Capacity>::data() const
	{
		return m_buf;
	}

	template <typename T, u64 Capacity>
	T& StackVector<T, Capacity>::front()
	{
		return m_buf[0];
	}

	template <typename T, u64 Capacity>
	const T& StackVector<T, Capacity>::front() const
	{
		return m_buf[0];
	}

	template <typename T, u64 Capacity>
	T& StackVector<T, Capacity>::back()
	{
		return m_buf[m_size - 1];
	}

	template <typename T, u64 Capacity>
	const T& StackVector<T, Capacity>::back() const
	{
		return m_buf[m_size - 1];
	}

	template <typename T, u64 Capacity>
	T& StackVector<T, Capacity>::at(u64 idx)
	{
		return m_buf[idx];
	}

	template <typename T, u64 Capacity>
	const T& StackVector<T, Capacity>::at(u64 idx) const
	{
		return m_buf[idx];
	}

	template <typename T, u64 Capacity>
	T& StackVector<T, Capacity>::operator [] (u64 idx)
	{
		return m_buf[idx];
	}

	template <typename T, u64 Capacity>
	const T& StackVector<T, Capacity>::operator [] (u64 idx) const
	{
		return m_buf[idx];
	}
}

#endif // STACK_VECTOR_H

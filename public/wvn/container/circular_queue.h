#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <wvn/common.h>
#include <wvn/assert.h>

namespace wvn
{
	/**
	 * Circular queue to which you can add
	 * new objects or pick the first one out of the queue,
	 * which then wraps back around after max capacity is reached,
	 * overwriting older items.
	 */
	template <typename T, u64 Size>
	class CircularQueue
	{
	public:
		CircularQueue();

		CircularQueue(const CircularQueue& other);
		CircularQueue(CircularQueue&& other) noexcept;

		CircularQueue& operator = (const CircularQueue& other);
		CircularQueue& operator = (CircularQueue&& other) noexcept;

		~CircularQueue();

		T& front();
		const T& front() const;
		T& back();
		const T& back() const;

		void clear();
		bool empty() const;
		u64 size() const;
		void swap(CircularQueue& other);

		T& push(const T& item);
		T pop();

		template <typename... Args>
		T& emplace(Args&&... args);

	private:
		T* m_ptr;
		u64 m_size;
	};

	template <typename T, u64 Size>
	CircularQueue<T, Size>::CircularQueue()
		: m_size(0)
	{
		m_ptr = new T[Size];
	}

	template <typename T, u64 Size>
	CircularQueue<T, Size>::CircularQueue(const CircularQueue<T, Size>& other)
	{
		this->CAPACITY = other.CAPACITY;
		this->m_size = other.m_size;

		this->m_ptr = new T[other.CAPACITY];

		for (int i = 0; i < other.m_size; i++)
			new(m_ptr + i) T(other.m_ptr[i]);
	}

	template <typename T, u64 Size>
	CircularQueue<T, Size>::CircularQueue(CircularQueue<T, Size>&& other) noexcept
	{
		this->CAPACITY = std::move(other.CAPACITY);
		this->m_size = std::move(other.m_size);
		this->m_ptr = std::move(other.m_ptr);

		other.m_size = 0;
		other.m_ptr = nullptr;
	}

	template <typename T, u64 Size>
	CircularQueue<T, Size>& CircularQueue<T, Size>::operator = (const CircularQueue<T, Size>& other)
	{
		this->CAPACITY = other.CAPACITY;
		this->m_size = other.m_size;

		this->m_ptr = new T[other.CAPACITY];

		for (int i = 0; i < other.m_size; i++) {
			new (m_ptr + i) T(other.m_ptr[i]);
		}

		return *this;
	}

	template <typename T, u64 Size>
	CircularQueue<T, Size>& CircularQueue<T, Size>::operator = (CircularQueue<T, Size>&& other) noexcept
	{
		this->CAPACITY = std::move(other.CAPACITY);
		this->m_size = std::move(other.m_size);
		this->m_ptr = std::move(other.m_ptr);

		other.m_size = 0;
		other.m_ptr = nullptr;

		return *this;
	}

	template <typename T, u64 Size>
	CircularQueue<T, Size>::~CircularQueue()
	{
		clear();
	}

	template <typename T, u64 Size>
	T& CircularQueue<T, Size>::front()
	{
		return *m_ptr;
	}

	template <typename T, u64 Size>
	const T& CircularQueue<T, Size>::front() const
	{
		return *m_ptr;
	}

	template <typename T, u64 Size>
	T& CircularQueue<T, Size>::back()
	{
		return *(m_ptr + m_size);
	}

	template <typename T, u64 Size>
	const T& CircularQueue<T, Size>::back() const
	{
		return *(m_ptr + m_size);
	}

	template <typename T, u64 Size>
	void CircularQueue<T, Size>::clear()
	{
		delete[] m_ptr;

		m_size = 0;
		m_ptr = nullptr;
	}

	template <typename T, u64 Size>
	bool CircularQueue<T, Size>::empty() const
	{
		return m_size == 0;
	}

	template <typename T, u64 Size>
	u64 CircularQueue<T, Size>::size() const
	{
		return m_size;
	}

	template <typename T, u64 Size>
	void CircularQueue<T, Size>::swap(CircularQueue<T, Size>& other)
	{
		T*  t_ptr  = m_ptr;
		u64 t_size = m_size;

		m_ptr  = other.m_ptr;
		m_size = other.m_size;

		other.m_ptr  = t_ptr;
		other.m_size = t_size;
	}

	template <typename T, u64 Size>
	T CircularQueue<T, Size>::pop()
	{
		WVN_ASSERT(m_size > 0, "[CIRQUEUE|DEBUG] Must not be empty when calling pop().");
		m_size--;
		if (m_size < 0) {
			m_size = Size - 1;
			return m_ptr[0];
		}
		return m_ptr[m_size + 1];
	}

	template <typename T, u64 Size>
	T& CircularQueue<T, Size>::push(const T& item)
	{
		WVN_ASSERT(m_size < Size, "[CIRQUEUE|DEBUG] Size must not be greater than max capacity.");
		m_size = (m_size + 1) % Size;
		new (m_ptr + m_size) T(std::move(item));
		return m_ptr[m_size];
	}

	template <typename T, u64 Size>
	template <typename... Args>
	T& CircularQueue<T, Size>::emplace(Args&&... args)
	{
		WVN_ASSERT(m_size < Size, "[CIRQUEUE|DEBUG] Size must not be greater than max capacity.");
		m_size = (m_size + 1) % Size;
		new (m_ptr + m_size) T(std::forward<Args>(args)...);
		return m_ptr[m_size];
	}
}

#endif // CIRCULAR_QUEUE_H

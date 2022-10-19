#pragma once

#include <wvn/util/types.h>

namespace wvn
{
	/*
	 * Queue of types to which you can add
	 * new objects or pick the first one out of the queue.
	 */
	template <typename T>
	class Queue
	{
	public:
		Queue(u64 capacity = 64);

		Queue(const Queue& other);
		Queue(Queue&& other) noexcept;

		Queue& operator = (const Queue& other);
		Queue& operator = (Queue&& other) noexcept;

		~Queue();

		T& front();
		const T& front() const;
		T& back();
		const T& back() const;

		void clear();
		bool empty() const;
		u64 size() const;

		T* push(const T& item);
		T pop();

		template <typename... Args>
		void emplace(Args&&... args);

	private:
		const u64 mc_capacity;

		T* m_front;
		u64 m_size;
	};

	template <typename T>
	Queue<T>::Queue(u64 capacity)
		: mc_capacity(capacity)
		, m_size(0)
	{
		WVN_ASSERT(capacity > 0, "Initial capacity must be greater than 0");

		m_front = new T[mc_capacity];
	}

	template <typename T>
	Queue<T>::Queue(const Queue<T>& other)
	{
		this->mc_capacity = other.mc_capacity;
		this->m_size = other.m_size;

		this->m_front = new T[other.mc_capacity];

		for (int i = 0; i < other.m_size; i++)
			new(m_front + i) T(other.m_front[i]);
	}

	template <typename T>
	Queue<T>::Queue(Queue<T>&& other) noexcept
	{
		this->mc_capacity = std::move(other.mc_capacity);
		this->m_size = std::move(other.m_size);
		this->m_front = std::move(other.m_front);

		other.m_size = 0;
		other.m_front = nullptr;
	}

	template <typename T>
	Queue<T>& Queue<T>::operator = (const Queue<T>& other)
	{
		this->mc_capacity = other.mc_capacity;
		this->m_size = other.m_size;

		this->m_front = new T[other.mc_capacity];

		for (int i = 0; i < other.m_size; i++)
			new (m_front + i) T(other.m_front[i]);

		return *this;
	}

	template <typename T>
	Queue<T>& Queue<T>::operator = (Queue<T>&& other) noexcept
	{
		this->mc_capacity = std::move(other.mc_capacity);
		this->m_size = std::move(other.m_size);
		this->m_front = std::move(other.m_front);

		other.m_size = 0;
		other.m_front = nullptr;

		return *this;
	}

	template <typename T>
	Queue<T>::~Queue()
	{
		delete[] m_front;
		m_size = 0;
	}

	template <typename T>
	T& Queue<T>::front()
	{
		return *m_front;
	}

	template <typename T>
	const T& Queue<T>::front() const
	{
		return *m_front;
	}

	template <typename T>
	T& Queue<T>::back()
	{
		return *(m_front + m_size);
	}

	template <typename T>
	const T& Queue<T>::back() const
	{
		return *(m_front + m_size);
	}

	template <typename T>
	void Queue<T>::clear()
	{
	}

	template <typename T>
	bool Queue<T>::empty() const
	{
		return m_size == 0;
	}

	template <typename T>
	u64 Queue<T>::size() const
	{
		return m_size;
	}

	template <typename T>
	T* Queue<T>::push(const T& item)
	{
		WVN_ASSERT(m_size < mc_capacity, "m_size must not be greater than max capacity");

		m_size++;
		new (m_front + m_size - 1) T(std::move(item));
		return &m_front[m_size - 1];
	}

	template <typename T>
	T Queue<T>::pop()
	{
		WVN_ASSERT(m_size > 0, "m_size must not be 0");

		m_size--;
		return *(m_front + m_size);
	}

	template <typename T>
	template <typename... Args>
	void Queue<T>::emplace(Args&&... args)
	{
		m_size++;
		new (m_front + m_size - 1) T(std::forward<Args>(args)...);
		return &m_front[m_size - 1];
	}
}

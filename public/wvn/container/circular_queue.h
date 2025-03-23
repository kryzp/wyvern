#ifndef CIRCULAR_QUEUE_H_
#define CIRCULAR_QUEUE_H_

#include <wvn/common.h>

namespace wvn
{
	/**
	 * Circular queue to which you can add
	 * new objects or pick the first one out of the queue,
	 * which then wraps back around after max capacity is reached,
	 * overwriting older items.
	 */
	template <typename T, u64 Capacity>
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
		bool full() const;

		u64 size() const;
		constexpr u64 capacity() const;

		T& push(const T& item);
		T pop();

		template <typename... Args>
		T& emplace(Args&&... args);

	private:
		s64 m_front;
		s64 m_rear;
		u64 m_size;
		T* m_buf;
	};

	template <typename T, u64 Capacity>
	CircularQueue<T, Capacity>::CircularQueue()
		: m_front(0)
		, m_rear(-1)
		, m_size(0)
		, m_buf(nullptr)
	{
		m_buf = new T[Capacity];
	}

	template <typename T, u64 Capacity>
	CircularQueue<T, Capacity>::CircularQueue(const CircularQueue<T, Capacity>& other)
	{
	}

	template <typename T, u64 Capacity>
	CircularQueue<T, Capacity>::CircularQueue(CircularQueue<T, Capacity>&& other) noexcept
	{
	}

	template <typename T, u64 Capacity>
	CircularQueue<T, Capacity>& CircularQueue<T, Capacity>::operator = (const CircularQueue<T, Capacity>& other)
	{
		return *this;
	}

	template <typename T, u64 Capacity>
	CircularQueue<T, Capacity>& CircularQueue<T, Capacity>::operator = (CircularQueue<T, Capacity>&& other) noexcept
	{
		return *this;
	}

	template <typename T, u64 Capacity>
	CircularQueue<T, Capacity>::~CircularQueue()
	{
		clear();

		if (m_buf) {
			::operator delete (m_buf, sizeof(T) * Capacity);
		}

		m_buf = nullptr;
		m_size = 0;
		m_front = 0;
		m_rear = -1;
	}

	template <typename T, u64 Capacity>
	T& CircularQueue<T, Capacity>::front()
	{
		return m_buf[m_front];
	}

	template <typename T, u64 Capacity>
	const T& CircularQueue<T, Capacity>::front() const
	{
		return m_buf[m_front];
	}

	template <typename T, u64 Capacity>
	T& CircularQueue<T, Capacity>::back()
	{
		return m_buf[m_rear];
	}

	template <typename T, u64 Capacity>
	const T& CircularQueue<T, Capacity>::back() const
	{
		return m_buf[m_rear];
	}

	template <typename T, u64 Capacity>
	void CircularQueue<T, Capacity>::clear()
	{
		for (int i = 0; i < m_size; i++) {
			m_buf[i].~T();
		}

		mem::set(m_buf, 0, m_size * sizeof(T));
		m_size = 0;

		m_front = 0;
		m_rear = -1;
	}

	template <typename T, u64 Capacity>
	T CircularQueue<T, Capacity>::pop()
	{
		wvn_ASSERT(m_size > 0, "[CIRQUE|DEBUG] must contain at least one element!");
		s64 tmp = m_front;
		m_front = (m_front + 1) % Capacity;
		m_size--;
		return m_buf[tmp];
	}

	template <typename T, u64 Capacity>
	T& CircularQueue<T, Capacity>::push(const T& item)
	{
		m_rear = (m_rear + 1) % Capacity;
		m_buf[m_rear] = item;
		m_size++;
		return m_buf[m_rear];
	}

	template <typename T, u64 Capacity>
	template <typename... Args>
	T& CircularQueue<T, Capacity>::emplace(Args&&... args)
	{
		m_rear = (m_rear + 1) % Capacity;
		new (m_buf + m_rear) T(std::forward<Args>(args)...);
		m_size++;
		return m_buf[m_rear];
	}

	template <typename T, u64 Capacity>
	bool CircularQueue<T, Capacity>::empty() const
	{
		return m_size == 0;
	}

	template <typename T, u64 Capacity>
	bool CircularQueue<T, Capacity>::full() const
	{
		return m_size == Capacity;
	}

	template <typename T, u64 Capacity>
	u64 CircularQueue<T, Capacity>::size() const
	{
		return m_size;
	}

	template <typename T, u64 Capacity>
	constexpr u64 CircularQueue<T, Capacity>::capacity() const
	{
		return Capacity;
	}
}

#endif // CIRCULAR_QUEUE_H_

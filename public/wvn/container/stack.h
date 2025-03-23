#ifndef STACK_H_
#define STACK_H_

#include <wvn/common.h>

namespace wvn
{
	/*
	 * Stack Container
	 */
	template <typename T, u64 Size = 128>
	class Stack
	{
	public:
		Stack();

		Stack(const Stack& other);
		Stack(Stack&& other) noexcept;

		Stack& operator = (const Stack& other);
		Stack& operator = (Stack&& other) noexcept;

		~Stack();

		T& push(const T& item);
		T& pop();

		template <typename... Args>
		T& emplace(Args&&... args);

		T& top();
		const T& top() const;

		bool empty() const;
		u64 size() const;

		u64 max_size() const;

	private:
		u64 m_ptr;
		T m_data[Size];
	};

	template <typename T, u64 Size>
	Stack<T, Size>::Stack()
		: m_ptr(0)
		, m_data{}
	{
	}

	template <typename T, u64 Size>
	Stack<T, Size>::Stack(const Stack& other)
		: Stack()
	{
		if (other.m_ptr > 0)
		{
			m_ptr = other.m_ptr;
			mem::copy(m_data, other.m_data, sizeof(T) * Size);
		}
	}

	template <typename T, u64 Size>
	Stack<T, Size>::Stack(Stack&& other) noexcept
		: Stack()
	{
		this->m_ptr = std::move(other.m_ptr);
		this->m_data = std::move(other.m_data);

		other.m_ptr = 0;
		other.m_data = nullptr;
	}

	template <typename T, u64 Size>
	Stack<T, Size>& Stack<T, Size>::operator = (const Stack& other)
	{
		if (other.m_ptr > 0)
		{
			m_ptr = other.m_ptr;
			mem::copy(m_data, other.m_data, sizeof(T) * Size);
		}

		return *this;
	}

	template <typename T, u64 Size>
	Stack<T, Size>& Stack<T, Size>::operator = (Stack&& other) noexcept
	{
		this->m_ptr = std::move(other.m_ptr);
		this->m_data = std::move(other.m_data);

		other.m_ptr = 0;
		other.m_data = nullptr;

		return *this;
	}

	template <typename T, u64 Size>
	Stack<T, Size>::~Stack()
	{
		mem::set(this->m_data, 0, Size * sizeof(T));
		this->m_ptr = 0;
	}

	template <typename T, u64 Size>
	T& Stack<T, Size>::push(const T& item)
	{
		m_data[m_ptr] = item;
		m_ptr++;
		return m_data[m_ptr - 1];
	}

	template <typename T, u64 Size>
	T& Stack<T, Size>::pop()
	{
		m_ptr--;
		return m_data[m_ptr];
	}

	template <typename T, u64 Size>
	template <typename... Args>
	T& Stack<T, Size>::emplace(Args&&... args)
	{
		new (m_data + m_ptr) T(std::forward<Args>(args)...);
		m_ptr++;
		return m_data[m_ptr - 1];
	}

	template <typename T, u64 Size>
	T& Stack<T, Size>::top()
	{
		wvn_ASSERT(m_ptr >= 1, "Must have at least one element on the stack!");
		return m_data[m_ptr - 1];
	}

	template <typename T, u64 Size>
	const T& Stack<T, Size>::top() const
	{
		wvn_ASSERT(m_ptr >= 1, "Must have at least one element on the stack!");
		return m_data[m_ptr - 1];
	}

	template <typename T, u64 Size>
	bool Stack<T, Size>::empty() const
	{
		return m_ptr <= 0;
	}

	template <typename T, u64 Size>
	u64 Stack<T, Size>::size() const
	{
		return m_ptr;
	}

	template <typename T, u64 Size>
	u64 Stack<T, Size>::max_size() const
	{
		return Size;
	}
}

#endif // STACK_H_

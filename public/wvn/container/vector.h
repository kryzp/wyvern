#ifndef VECTOR_H
#define VECTOR_H

#include <initializer_list>
#include <new>

#include <wvn/common.h>

// todo: does resizing work downwards
// why don't I call resize(m_size - 1) in the pop_...() calls?????

namespace wvn
{
	/**
	 * Dynamically sized array.
	 */
	template <typename T>
	class Vector
	{
    public:
		class Iterator
		{
			friend class Vector<T>;

		public:
			Iterator() : m_ptr(nullptr) { }
			Iterator(T* init) : m_ptr(init) { }
			~Iterator() = default;
			T& operator * () const { return *m_ptr; }
			T* operator -> () const { return m_ptr; }
			Iterator& operator ++ () { m_ptr++; return *this; }
			Iterator& operator -- () { m_ptr--; return *this; }
			Iterator& operator ++ (int) { m_ptr++; return *this; }
			Iterator& operator -- (int) { m_ptr--; return *this; }
			bool operator == (const Iterator& other) const { return this->m_ptr == other.m_ptr; }
			bool operator != (const Iterator& other) const { return this->m_ptr != other.m_ptr; }
		private:
			T* m_ptr;
		};

		class ConstIterator
		{
			friend class Vector<T>;

		public:
			ConstIterator() : m_ptr(nullptr) { }
			ConstIterator(const T* init) : m_ptr(init) { }
			~ConstIterator() = default;
			const T& operator * () const { return *m_ptr; }
			const T* operator -> () const { return m_ptr; }
			ConstIterator& operator ++ () { m_ptr++; return *this; }
			ConstIterator& operator -- () { m_ptr--; return *this; }
			ConstIterator& operator ++ (int) { m_ptr++; return *this; }
			ConstIterator& operator -- (int) { m_ptr--; return *this; }
			bool operator == (const ConstIterator& other) const { return this->m_ptr == other.m_ptr; }
			bool operator != (const ConstIterator& other) const { return this->m_ptr != other.m_ptr; }
		private:
			const T* m_ptr;
		};

		class ReverseIterator
		{
			friend class Vector<T>;

		public:
			ReverseIterator() : m_ptr(nullptr) { }
			ReverseIterator(T* init) : m_ptr(init) { }
			~ReverseIterator() = default;
			T& operator * () const { return *m_ptr; }
			T* operator -> () const { return m_ptr; }
			ReverseIterator& operator ++ () { m_ptr--; return *this; }
			ReverseIterator& operator -- () { m_ptr++; return *this; }
			ReverseIterator& operator ++ (int) { m_ptr--; return *this; }
			ReverseIterator& operator -- (int) { m_ptr++; return *this; }
			bool operator == (const ReverseIterator& other) const { return this->m_ptr == other.m_ptr; }
			bool operator != (const ReverseIterator& other) const { return this->m_ptr != other.m_ptr; }
		private:
			T* m_ptr;
		};

		class ReverseConstIterator
		{
			friend class Vector<T>;

		public:
			ReverseConstIterator() : m_ptr(nullptr) { }
			ReverseConstIterator(const T* init) : m_ptr(init) { }
			~ReverseConstIterator() = default;
			const T& operator * () const { return *m_ptr; }
			const T* operator -> () const { return m_ptr; }
			ReverseConstIterator& operator ++ () { m_ptr--; return *this; }
			ReverseConstIterator& operator -- () { m_ptr++; return *this; }
			ReverseConstIterator& operator ++ (int) { m_ptr--; return *this; }
			ReverseConstIterator& operator -- (int) { m_ptr++; return *this; }
			bool operator == (const ReverseConstIterator& other) const { return this->m_ptr == other.m_ptr; }
			bool operator != (const ReverseConstIterator& other) const { return this->m_ptr != other.m_ptr; }
		private:
			const T* m_ptr;
		};

        Vector();

        Vector(std::initializer_list<T> data);
        Vector(u64 initial_capacity);
        Vector(u64 initial_capacity, const T& initial_element);
		Vector(T* buf, u64 length);
		Vector(const Iterator& begin, const Iterator& end);

        Vector(const Vector& other);
        Vector(Vector&& other) noexcept;

        Vector& operator = (const Vector& other);
        Vector& operator = (Vector&& other) noexcept;

        ~Vector();

        void allocate(u64 capacity);
        void resize(u64 new_size);
        void expand(u64 amount = 1);
        void fill(byte value);

		void erase(u64 index, u64 amount = 1);
		void erase(Iterator it, u64 amount = 1);

		Iterator find(const T& elem);

		T* data();
		const T* data() const;

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

		bool any() const;
		bool empty() const;

		T& front();
		const T& front() const;
		T& back();
		const T& back() const;

        Iterator begin();
        ConstIterator begin() const;
        Iterator end();
        ConstIterator end() const;

		ReverseIterator rbegin();
		ReverseConstIterator rbegin() const;
		ReverseIterator rend();
		ReverseConstIterator rend() const;

		ConstIterator cbegin() const;
		ConstIterator cend() const;
		ReverseConstIterator crbegin() const;
		ReverseConstIterator crend() const;

		T& at(u64 idx);
        const T& at(u64 idx) const;

        T& operator [] (u64 idx);
        const T& operator [] (u64 idx) const;

    private:
        T* m_buf;
        u64 m_size;
        u64 m_capacity;
	};

    template <typename T>
    Vector<T>::Vector()
		: m_buf(nullptr)
        , m_size(0)
        , m_capacity(0)
    {
    }
    
    template <typename T>
    Vector<T>::Vector(std::initializer_list<T> data)
        : Vector()
    {
        allocate(data.size());
        m_size = data.size();

        for (u64 i = 0; i < m_size; i++) {
            new (m_buf + i) T(data.begin()[i]);
        }
    }

    template <typename T>
    Vector<T>::Vector(u64 initial_capacity)
        : Vector()
    {
        allocate(initial_capacity);
        m_size = initial_capacity;

        for (u64 i = 0; i < m_capacity; i++) {
            new (m_buf + i) T();
        }
    }

    template <typename T>
    Vector<T>::Vector(u64 initial_capacity, const T& initial_element)
        : Vector()
    {
        allocate(initial_capacity);
        m_size = initial_capacity;

        for (u64 i = 0; i < m_capacity; i++) {
            new (m_buf + i) T(initial_element);
        }
    }

	template <typename T>
	Vector<T>::Vector(T* buf, u64 length)
		: Vector()
	{
		allocate(length);
		m_size = length;

		for (u64 i = 0; i < length; i++) {
			new (m_buf + i) T(buf[i]);
        }
	}

	template <typename T>
	Vector<T>::Vector(const Iterator& begin, const Iterator& end)
		: Vector()
	{
		allocate(end.m_ptr - begin.m_ptr);
		m_size = end.m_ptr - begin.m_ptr;

		for (u64 i = 0; i < m_size; i++) {
			new (m_buf + i) T(begin.m_ptr[i]);
        }
	}

    template <typename T>
    Vector<T>::Vector(const Vector& other)
        : Vector()
    {
        if (other.m_capacity <= 0) {
			return;
		}

		allocate(other.m_capacity);
		clear();
		m_size = other.size();

		for (u64 i = 0; i < other.m_size; i++) {
			new (m_buf + i) T(other.m_buf[i]);
		}
    }

    template <typename T>
    Vector<T>::Vector(Vector&& other) noexcept
		: Vector()
    {
        this->m_capacity = std::move(other.m_capacity);
        this->m_size = std::move(other.m_size);
        this->m_buf = std::move(other.m_buf);

        other.m_capacity = 0;
        other.m_size = 0;
        other.m_buf = nullptr;
    }
    
    template <typename T>
    Vector<T>& Vector<T>::operator = (const Vector& other)
    {
		allocate(other.m_capacity);
		clear();

		m_size = other.m_size;

		for (u64 i = 0; i < other.m_size; i++) {
			new (m_buf + i) T(other.m_buf[i]);
		}

        return *this;
    }
    
    template <typename T>
    Vector<T>& Vector<T>::operator = (Vector&& other) noexcept
    {
		clear();

		if (m_buf) {
			::operator delete (m_buf, sizeof(T) * m_capacity);
		}

		this->m_capacity = std::move(other.m_capacity);
		this->m_size = std::move(other.m_size);
		this->m_buf = std::move(other.m_buf);

		other.m_capacity = 0;
		other.m_size = 0;
		other.m_buf = nullptr;

        return *this;
    }

    template <typename T>
    Vector<T>::~Vector()
    {
        clear();

		if (m_buf) {
			::operator delete (m_buf, sizeof(T) * m_capacity);
		}

        m_buf = nullptr;
        m_capacity = 0;
        m_size = 0;
    }

    template <typename T>
    void Vector<T>::clear()
    {
        for (int i = 0; i < m_size; i++) {
            m_buf[i].~T();
		}

		mem::set(m_buf, 0, m_size * sizeof(T));
        m_size = 0;
    }

    template <typename T>
    void Vector<T>::allocate(u64 capacity)
    {
        if (capacity <= m_capacity) {
			return;
		}

		u64 new_capacity = capacity > 8 ? capacity : 8;

		while (new_capacity < capacity) {
			new_capacity *= 2;
		}

		T* new_buf = (T*)::operator new (sizeof(T) * new_capacity);
		mem::set(new_buf, 0, sizeof(T) * new_capacity);

		for (int i = 0; i < m_size; i++) {
			new (new_buf + i) T(std::move(m_buf[i]));
		}

		if (m_buf) {
			::operator delete (m_buf, sizeof(T) * m_capacity);
		}

		m_buf = new_buf;
		m_capacity = new_capacity;
    }

    template <typename T>
    void Vector<T>::resize(u64 new_size)
    {
        if (new_size < m_size) {
            erase(new_size, m_size - new_size);
        } else if (new_size > m_size) {
            expand(new_size - m_size);
        }

		m_size = new_size;
    }

    template <typename T>
    void Vector<T>::expand(u64 amount)
    {
        WVN_ASSERT(amount > 0, "[VECTOR|DEBUG] Expand amount must be higher than 0");

        allocate(m_size + amount);

        for (int i = 0; i < amount; i++) {
            new (m_buf + m_size + i) T();
        }
    }

    template <typename T>
    void Vector<T>::fill(byte value)
    {
        mem::set(m_buf, value, sizeof(T) * m_capacity);
    }

	template <typename T>
	void Vector<T>::erase(u64 index, u64 amount)
	{
		if (amount <= 0) {
			return;
		}

		for (u64 i = 0; i < amount; i++) {
			m_buf[i + index].~T();
		}

		for (u64 i = 0; i < m_size - amount; i++) {
			m_buf[i + index] = std::move(m_buf[i + index + amount]);
        }

		m_size -= amount;
	}

	template <typename T>
	void Vector<T>::erase(Iterator it, u64 amount)
	{
		if (amount <= 0) {
			return;
		}

		for (int i = 0; i < m_size - amount; i++) {
			*(it.m_ptr + i) = std::move(*(it.m_ptr + i + amount));
        }

		for (int i = m_size - amount; i < m_size; i++) {
			it->~T();
        }

		m_size -= amount;
	}

	template <typename T>
	typename Vector<T>::Iterator Vector<T>::find(const T& elem)
	{
		for (u64 i = 0; i < m_size; i++) {
			if (m_buf[i] == elem) {
				return Iterator(&m_buf[i]);
			}
		}

		return end();
	}

    template <typename T>
    T& Vector<T>::push_front(const T& item)
    {
        resize(m_size + 1);
        mem::move(m_buf + 1, m_buf, sizeof(T) * m_size);
        new (m_buf) T(std::move(item));
        m_size++;
		return m_buf[0];
    }

    template <typename T>
    T& Vector<T>::push_back(const T& item)
    {
        resize(m_size + 1);
        new (m_buf + m_size - 1) T(std::move(item));
		return m_buf[m_size - 1];
    }

    template <typename T>
    T Vector<T>::pop_front()
    {
        T item = std::move(m_buf[0]);
        m_buf[0].~T();
		mem::move(m_buf, m_buf + 1, m_size - 1);
        m_size--;
        return item;
    }

    template <typename T>
    T Vector<T>::pop_back()
    {
        T item = std::move(m_buf[m_size - 1]);
        m_buf[m_size - 1].~T();
        m_size--;
        return item;
    }

	template <typename T>
	template <typename... Args>
	T& Vector<T>::emplace_front(Args&&... args)
	{
		resize(m_size + 1);
		mem::move(m_buf + 1, m_buf, sizeof(T) * (m_size - 1));
		new (m_buf) T(std::forward<Args>(args)...);
		return m_buf[0];
	}

	template <typename T>
	template <typename... Args>
	T& Vector<T>::emplace_back(Args&&... args)
	{
		resize(m_size + 1);
		new (m_buf + m_size - 1) T(std::forward<Args>(args)...);
		return m_buf[m_size - 1];
	}

	template <typename T>
	T* Vector<T>::data()
	{
		return m_buf;
	}

	template <typename T>
	const T* Vector<T>::data() const
	{
		return m_buf;
	}

	template <typename T>
	T& Vector<T>::front()
	{
		return m_buf[0];
	}

	template <typename T>
	const T& Vector<T>::front() const
	{
		return m_buf[0];
	}

	template <typename T>
	T& Vector<T>::back()
	{
		return m_buf[m_size - 1];
	}

	template <typename T>
	const T& Vector<T>::back() const
	{
		return m_buf[m_size - 1];
	}

    template <typename T>
    u64 Vector<T>::size() const
    {
        return m_size;
    }

	template <typename T>
	bool Vector<T>::any() const
	{
		return m_size != 0;
	}

	template <typename T>
	bool Vector<T>::empty() const
	{
		return m_size == 0;
	}

	template <typename T>
	typename Vector<T>::Iterator Vector<T>::begin()
	{
		return Iterator(m_buf);
	}

	template <typename T>
	typename Vector<T>::ConstIterator Vector<T>::begin() const
	{
		return ConstIterator(m_buf);
	}

	template <typename T>
	typename Vector<T>::Iterator Vector<T>::end()
	{
		return Iterator(m_buf + m_size);
	}

	template <typename T>
	typename Vector<T>::ConstIterator Vector<T>::end() const
	{
		return ConstIterator(m_buf + m_size);
	}

	template <typename T>
	typename Vector<T>::ReverseIterator Vector<T>::rbegin()
	{
		return ReverseIterator(m_buf + m_size - 1);
	}

	template <typename T>
	typename Vector<T>::ReverseConstIterator Vector<T>::rbegin() const
	{
		return ReverseConstIterator(m_buf + m_size - 1);
	}

	template <typename T>
	typename Vector<T>::ReverseIterator Vector<T>::rend()
	{
		return ReverseIterator(m_buf - 1);
	}

	template <typename T>
	typename Vector<T>::ReverseConstIterator Vector<T>::rend() const
	{
		return ReverseConstIterator(m_buf - 1);
	}

	template <typename T>
	typename Vector<T>::ConstIterator Vector<T>::cbegin() const
	{
		return ConstIterator(m_buf);
	}

	template <typename T>
	typename Vector<T>::ConstIterator Vector<T>::cend() const
	{
		return ConstIterator(m_buf + m_size);
	}

	template <typename T>
	typename Vector<T>::ReverseConstIterator Vector<T>::crbegin() const
	{
		return ReverseConstIterator(m_buf + m_size - 1);
	}

	template <typename T>
	typename Vector<T>::ReverseConstIterator Vector<T>::crend() const
	{
		return ReverseConstIterator(m_buf - 1);
	}

    template <typename T>
    T& Vector<T>::at(u64 idx)
    {
        return m_buf[idx];
    }

    template <typename T>
    const T& Vector<T>::at(u64 idx) const
    {
        return m_buf[idx];
    }

    template <typename T>
    T& Vector<T>::operator [] (u64 idx)
    {
        return m_buf[idx];
    }

    template <typename T>
    const T& Vector<T>::operator [] (u64 idx) const
    {
        return m_buf[idx];
    }
}

#endif // VECTOR_H

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

namespace wvn
{
	/**
	 * List in which each element or node points to the next node.
	 */
	template <typename T>
	class LinkedList
	{
	public:
		struct Link
		{
			Link* next = nullptr;
			Link* prev = nullptr;
			T data;
		};

		struct Iterator
		{
			Iterator() : m_ptr(nullptr) { }
			Iterator(Link* init) : m_ptr(init) { }
			~Iterator() = default;
			T& operator * () const { return m_ptr->data; }
			T* operator -> () const { return &m_ptr->data; }
			Iterator& operator ++ () { if (m_ptr) { m_ptr = m_ptr->next; } return *this; }
			Iterator& operator -- () { if (m_ptr) { m_ptr = m_ptr->prev; } return *this; }
			Iterator& operator ++ (int) { if (m_ptr) { m_ptr = m_ptr->next; } return *this; }
			Iterator& operator -- (int) { if (m_ptr) { m_ptr = m_ptr->prev; } return *this; }
			bool operator == (const Iterator& other) const { return this->m_ptr == other.m_ptr; }
			bool operator != (const Iterator& other) const { return this->m_ptr != other.m_ptr; }
		private:
			Link* m_ptr;
		};

		struct ConstIterator
		{
			ConstIterator() : m_ptr(nullptr) { }
			ConstIterator(const Link* init) : m_ptr(init) { }
			~ConstIterator() = default;
			const T& operator * () const { return m_ptr->data; }
			const T* operator -> () const { return &m_ptr->data; }
			ConstIterator& operator ++ () { if (m_ptr) { m_ptr = m_ptr->next; } return *this; }
			ConstIterator& operator -- () { if (m_ptr) { m_ptr = m_ptr->prev; } return *this; }
			ConstIterator& operator ++ (int) { if (m_ptr) { m_ptr = m_ptr->next; } return *this; }
			ConstIterator& operator -- (int) { if (m_ptr) { m_ptr = m_ptr->prev; } return *this; }
			bool operator == (const Iterator& other) const { return this->m_ptr == other.m_ptr; }
			bool operator != (const Iterator& other) const { return this->m_ptr != other.m_ptr; }
		private:
			const Link* m_ptr;
		};

		LinkedList();
		~LinkedList();

		Link* add(T item);
		void remove(T item);

		T& first();
		const T& first() const;
		T& last();
		const T& last() const;

		Iterator begin();
		ConstIterator begin() const;
		Iterator end();
		ConstIterator end() const;

		ConstIterator cbegin() const;
		ConstIterator cend() const;

	private:
		void remove(Link* link);
		Link* find(T item) const;

		Link* m_first;
		Link* m_last;
	};

	template <typename T>
	LinkedList<T>::LinkedList()
		: m_first(nullptr)
		, m_last(nullptr)
	{
	}

	template <typename T>
	LinkedList<T>::~LinkedList()
	{
		Link* node = m_first;
		while (node)
		{
			Link* next = node->next;
			delete node;
			node = next;
		}

		m_first = nullptr;
		m_last = nullptr;
	}

	template <typename T>
	typename LinkedList<T>::Link* LinkedList<T>::add(T item)
	{
		Link* node = new Link();
		node->data = item;

		if (m_last)
		{
			m_last->next = node;
			node->prev = m_last;
			node->next = nullptr;
			m_last = node;
		}
		else
		{
			node->next = nullptr;
			node->prev = nullptr;

			m_first = node;
			m_last = node;
		}

		return node;
	}

	template <typename T>
	void LinkedList<T>::remove(Link* link)
	{
		if (!link)
			return;

		if (link->next)
			link->next->prev = link->prev;

		if (link->prev)
			link->prev->next = link->next;

		if (link == m_first)
			m_first = link->next;

		if (link == m_last)
			m_last = link->prev;

		link->next = nullptr;
		link->prev = nullptr;

		delete link;
	}

	template <typename T>
	void LinkedList<T>::remove(T item)
	{
		remove(find(item));
	}

	template <typename T>
	typename LinkedList<T>::Link* LinkedList<T>::find(T item) const
	{
		for (auto* node = m_first; node != nullptr; node = node->next) {
			if (node->data == item) {
				return node;
			}
		}

		return nullptr;
	}

	template <typename T>
	T& LinkedList<T>::first()
	{
		return m_first->data;
	}

	template <typename T>
	const T& LinkedList<T>::first() const
	{
		return m_first->data;
	}

	template <typename T>
	T& LinkedList<T>::last()
	{
		return m_last->data;
	}

	template <typename T>
	const T& LinkedList<T>::last() const
	{
		return m_last->data;
	}

	template <typename T>
	typename LinkedList<T>::Iterator LinkedList<T>::begin()
	{
		return Iterator(m_first);
	}

	template <typename T>
	typename LinkedList<T>::ConstIterator LinkedList<T>::begin() const
	{
		return ConstIterator(m_first);
	}

	template <typename T>
	typename LinkedList<T>::Iterator LinkedList<T>::end()
	{
		return Iterator(nullptr);
	}

	template <typename T>
	typename LinkedList<T>::ConstIterator LinkedList<T>::end() const
	{
		return ConstIterator(nullptr);
	}

	template <typename T>
	typename LinkedList<T>::ConstIterator LinkedList<T>::cbegin() const
	{
		return ConstIterator(m_first);
	}

	template <typename T>
	typename LinkedList<T>::ConstIterator LinkedList<T>::cend() const
	{
		return ConstIterator(nullptr);
	}
}

#endif // LINKED_LIST_H

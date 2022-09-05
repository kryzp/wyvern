#pragma once

namespace wvn
{
	template <typename T>
	class LinkedList
	{
	public:
		struct Node
		{
			Node* next = nullptr;
			Node* prev = nullptr;
			T data;
		};

		struct Iterator
		{
			Iterator() : m_node(nullptr) { }
			Iterator(Node* init) : m_node(init) { }
			~Iterator() = default;
			T& operator * () const { return m_node->data; }
			T* operator -> () const { return &m_node->data; }
			Iterator& operator ++ () { if (m_node) { m_node = m_node->next; } return *this; }
			Iterator& operator -- () { if (m_node) { m_node = m_node->prev; } return *this; }
			Iterator& operator ++ (int) { if (m_node) { m_node = m_node->next; } return *this; }
			Iterator& operator -- (int) { if (m_node) { m_node = m_node->prev; } return *this; }
			bool operator == (const Iterator& other) const { return this->m_node == other.m_node; }
			bool operator != (const Iterator& other) const { return this->m_node != other.m_node; }
		private:
			Node* m_node;
		};

		struct ConstIterator
		{
			ConstIterator() : m_node(nullptr) { }
			ConstIterator(const Node* init) : m_node(init) { }
			~ConstIterator() = default;
			const T& operator * () const { return m_node->data; }
			const T* operator -> () const { return &m_node->data; }
			ConstIterator& operator ++ () { if (m_node) { m_node = m_node->next; } return *this; }
			ConstIterator& operator -- () { if (m_node) { m_node = m_node->prev; } return *this; }
			ConstIterator& operator ++ (int) { if (m_node) { m_node = m_node->next; } return *this; }
			ConstIterator& operator -- (int) { if (m_node) { m_node = m_node->prev; } return *this; }
			bool operator == (const Iterator& other) const { return this->m_node == other.m_node; }
			bool operator != (const Iterator& other) const { return this->m_node != other.m_node; }
		private:
			const Node* m_node;
		};

		LinkedList();
		~LinkedList();

		Node* add(T item);
		void remove(Node* node);
		void remove(T item);
		Node* find(T item) const;

		Iterator begin();
		ConstIterator begin() const;
		Iterator end();
		ConstIterator end() const;

		Node* first;
		Node* last;
	};

	template <typename T>
	LinkedList<T>::LinkedList()
		: first(nullptr)
		, last(nullptr)
	{
	}

	template <typename T>
	LinkedList<T>::~LinkedList()
	{
		Node* node = first;
		while (node)
		{
			Node* next = node->next;
			delete node;
			node = next;
		}

		first = nullptr;
		last = nullptr;
	}

	template <typename T>
	typename LinkedList<T>::Node* LinkedList<T>::add(T item)
	{
		Node* node = new Node();
		node->data = item;

		if (last)
		{
			last->next = node;
			node->prev = last;
			node->next = nullptr;
			last = node;
		}
		else
		{
			node->next = nullptr;
			node->prev = nullptr;

			first = node;
			last = node;
		}

		return node;
	}

	template <typename T>
	void LinkedList<T>::remove(Node* node)
	{
		if (!node)
			return;

		if (node->next)
			node->next->prev = node->prev;

		if (node->prev)
			node->prev->next = node->next;

		if (node == first)
			first = node->next;

		if (node == last)
			last = node->prev;

		node->next = nullptr;
		node->prev = nullptr;

		delete node;
	}

	template <typename T>
	void LinkedList<T>::remove(T item)
	{
		remove(find(item));
	}

	template <typename T>
	typename LinkedList<T>::Node* LinkedList<T>::find(T item) const
	{
		for (auto* node = first; node != nullptr; node = node->next)
		{
			if (node->data == item)
				return node;
		}

		return nullptr;
	}

	template <typename T>
	typename LinkedList<T>::Iterator LinkedList<T>::begin()
	{
		return Iterator(first);
	}

	template <typename T>
	typename LinkedList<T>::ConstIterator LinkedList<T>::begin() const
	{
		return ConstIterator(first);
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
}

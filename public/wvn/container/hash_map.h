#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <wvn/common.h>
#include <wvn/container/pair.h>

// std::hash
#include <functional>

namespace wvn
{
	/**
	 * Dictionary structure that uses a hash function
	 * to index the different elements inside it.
	 */
	template <typename TKey, typename TValue>
	class HashMap
	{
	public:
		using KeyValuePair = Pair<TKey, TValue>;

		struct Element
		{
			Element() : data(), next(nullptr), prev(nullptr) { }
			Element(const KeyValuePair& p) : data(p), next(nullptr), prev(nullptr) { }
			Element(Element&& other) noexcept : data(std::move(other.data)), next(std::move(other.next)), prev(std::move(other.prev)) { };
			KeyValuePair data;
			Element* next;
			Element* prev;
		};

		struct Iterator
		{
			Iterator() : m_elem(nullptr) { }
			Iterator(Element* init) : m_elem(init) { }
			~Iterator() = default;
			KeyValuePair& operator * () const { return m_elem->data; }
			KeyValuePair* operator -> () const { return &m_elem->data; }
			Iterator& operator ++ () { if (m_elem) { m_elem = m_elem->next; } return *this; }
			Iterator& operator -- () { if (m_elem) { m_elem = m_elem->prev; } return *this; }
			Iterator& operator ++ (int) { if (m_elem) { m_elem = m_elem->next; } return *this; }
			Iterator& operator -- (int) { if (m_elem) { m_elem = m_elem->prev; } return *this; }
			bool operator == (const Iterator& other) const { return this->m_elem == other.m_elem; }
			bool operator != (const Iterator& other) const { return this->m_elem != other.m_elem; }
		private:
			Element* m_elem;
		};

		struct ConstIterator
		{
			ConstIterator() : m_elem(nullptr) { }
			ConstIterator(const Element* init) : m_elem(init) { }
			~ConstIterator() = default;
			const KeyValuePair& operator * () const { return m_elem->data; }
			const KeyValuePair* operator -> () const { return &m_elem->data; }
			ConstIterator& operator ++ () { if (m_elem) { m_elem = m_elem->next; } return *this; }
			ConstIterator& operator -- () { if (m_elem) { m_elem = m_elem->prev; } return *this; }
			ConstIterator& operator ++ (int) { if (m_elem) { m_elem = m_elem->next; } return *this; }
			ConstIterator& operator -- (int) { if (m_elem) { m_elem = m_elem->prev; } return *this; }
			bool operator == (const ConstIterator& other) const { return this->m_elem == other.m_elem; }
			bool operator != (const ConstIterator& other) const { return this->m_elem != other.m_elem; }
		private:
			const Element* m_elem;
		};

		HashMap();
		HashMap(int initial_capacity);

		HashMap(const HashMap& other);
		HashMap(HashMap&& other) noexcept;

		HashMap& operator = (const HashMap& other);
		HashMap& operator = (HashMap&& other) noexcept;

		~HashMap();

		void insert(const KeyValuePair& pair);
		void erase(const TKey& key);
		void clear();

		TValue& get(const TKey& key);
		const TValue& get(const TKey& key) const;

		bool contains(const TKey& key);

		int element_count() const;
		int capacity() const;
		bool is_empty() const;

		Element* first();
		const Element* first() const;
		Element* last();
		const Element* last() const;

		Iterator begin();
		ConstIterator begin() const;
		Iterator end();
		ConstIterator end() const;

		ConstIterator cbegin() const;
		ConstIterator cend() const;

		TValue& operator [] (const TKey& idx);
		const TValue& operator [] (const TKey& idx) const;

	private:
		int index_of(const TKey& key) const;
		void realloc();
		void realign_ptrs();

		void _insert(const KeyValuePair& pair);

		Element** m_elements;
		int m_element_count;
		int m_capacity;
	};

	template <typename TKey, typename TValue>
	HashMap<TKey, TValue>::HashMap()
		: m_elements(nullptr)
		, m_element_count(0)
		, m_capacity(0)
	{
		realloc();
	}

	template <typename TKey, typename TValue>
	HashMap<TKey, TValue>::HashMap(int initial_capacity)
		: m_elements(nullptr)
		, m_element_count(0)
		, m_capacity(initial_capacity)
	{
		realloc();
	}

	template <typename TKey, typename TValue>
	HashMap<TKey, TValue>::HashMap(const HashMap& other)
	{
		this->m_elements = nullptr;
		this->m_element_count = other.m_element_count;
		this->m_capacity = other.m_capacity;

		realloc();

		if (!other.m_elements) {
			return;
		}

		for (int i = 0; i < other.m_capacity; i++)
		{
			if (other.m_elements[i])
			{
				Element* elem_ptr = other.m_elements[i];

				while (elem_ptr) {
					_insert(elem_ptr->data);
					elem_ptr = elem_ptr->next;
				}

				break;
			}
		}

		realign_ptrs();
	}

	template <typename TKey, typename TValue>
	HashMap<TKey, TValue>::HashMap(HashMap&& other) noexcept
	{
		this->m_elements = std::move(other.m_elements);
		this->m_element_count = std::move(other.m_element_count);
		this->m_capacity = std::move(other.m_capacity);

		other.m_elements = nullptr;
		other.m_element_count = 0;
		other.m_capacity = 0;
	}

	template <typename TKey, typename TValue>
	HashMap<TKey, TValue>& HashMap<TKey, TValue>::operator = (const HashMap& other)
	{
		this->m_element_count = other.m_element_count;
		this->m_capacity = other.m_capacity;

		realloc();

		for (int i = 0; i < other.m_capacity; i++)
		{
			Element* elem_ptr = other.m_elements[i];
			if (elem_ptr) {
				_insert(elem_ptr->data);
			}
		}

		realign_ptrs();

		return *this;
	}

	template <typename TKey, typename TValue>
	HashMap<TKey, TValue>& HashMap<TKey, TValue>::operator = (HashMap&& other) noexcept
	{
		this->m_elements = std::move(other.m_elements);
		this->m_element_count = std::move(other.m_element_count);
		this->m_capacity = std::move(other.m_capacity);

		other.m_elements = nullptr;
		other.m_element_count = 0;
		other.m_capacity = 0;

		return *this;
	}

	template <typename TKey, typename TValue>
	HashMap<TKey, TValue>::~HashMap()
	{
		Element* e = m_elements[0];

		while (e)
		{
			Element* next = e->next;
			delete e;
			e = next;
		}

		delete[] m_elements;

		m_capacity = 0;
		m_element_count = 0;
	}

	template <typename TKey, typename TValue>
	void HashMap<TKey, TValue>::insert(const KeyValuePair& pair)
	{
		_insert(pair);
		realign_ptrs();
		m_element_count++;
	}

	template <typename TKey, typename TValue>
	void HashMap<TKey, TValue>::_insert(const KeyValuePair& pair)
	{
		auto idx = index_of(pair.first);

		Element* b = m_elements[idx];

		if (b)
		{
			while (b->next) {
				b = b->next;
			}

			b->next = new Element(pair);
			b->next->prev = b;
		}
		else
		{
			m_elements[idx] = new Element(pair);
		}
	}

	template <typename TKey, typename TValue>
	void HashMap<TKey, TValue>::erase(const TKey& key)
	{
		Element* b = m_elements[index_of(key)];

		while (b)
		{
			if (b->data.first == key)
			{
				if (b == m_elements[index_of(key)]) {
					m_elements[index_of(key)] = b->next;
				}

				if (b->next) {
					b->next->prev = b->prev;
				}

				if (b->prev) {
					b->prev->next = b->next;
				}

				::operator delete(b, sizeof(Element));

				m_element_count--;

				realign_ptrs();

				return;
			}

			b = b->next;
		}

	}

	template <typename TKey, typename TValue>
	void HashMap<TKey, TValue>::clear()
	{
		m_element_count = 0;
	}

	template <typename TKey, typename TValue>
	void HashMap<TKey, TValue>::realloc()
	{
		int old_size = m_capacity;

		if (m_capacity < 8) {
			m_capacity = 8;
		}

		while (m_element_count >= m_capacity) {
			m_capacity *= 2;
		}

		Element** new_buf = new Element*[m_capacity];
		mem::set(new_buf, 0, sizeof(Element*) * m_capacity);

		if (m_elements)
		{
			for (int i = 0; i < old_size; i++)
			{
				if (m_elements[i])
				{
					int idx = index_of(m_elements[i]->data.first);
					new_buf[idx] = new Element(std::move(*m_elements[i]));
				}
			}
		}

		delete[] m_elements;
		m_elements = new_buf;

		realign_ptrs();
	}

	template <typename TKey, typename TValue>
	void HashMap<TKey, TValue>::realign_ptrs()
	{
		Element* f = first();
		Element* l = last();

		for (int i = 0; i < m_capacity; i++)
		{
			Element* bkt = m_elements[i];

			if (!bkt) {
				continue;
			}

			if (bkt != l)
			{
				for (int j = 0; j < m_capacity - 1; j++)
				{
					int check_idx = (i + j + 1) % m_capacity;

					if (m_elements[check_idx])
					{
						bkt->next = m_elements[check_idx];
						break;
					}
				}
			}

			if (bkt != f)
			{
				for (int j = 0; j < m_capacity - 1; j++)
				{
					int check_idx = m_capacity - 1 - j + i;

					if (check_idx < 0) {
						check_idx += m_capacity;
					} else if (check_idx >= m_capacity) {
						check_idx -= m_capacity;
					}

					if (m_elements[check_idx])
					{
						bkt->prev = m_elements[check_idx];
						break;
					}
				}
			}
		}
	}

	template <typename TKey, typename TValue>
	TValue& HashMap<TKey, TValue>::get(const TKey& key)
	{
		Element* b = m_elements[index_of(key)];

		while (b)
		{
			if (b->data.first == key) {
				return b->data.second;
			}

			b = b->next;
		}

		WVN_ERROR("[HASHMAP|DEBUG] Could not find bucket matching key.");
		return m_elements[0]->data.second;
	}

	template <typename TKey, typename TValue>
	const TValue& HashMap<TKey, TValue>::get(const TKey& key) const
	{
		Element* b = m_elements[index_of(key)];

		while (b)
		{
			if (b->data.first == key) {
				return b->data.second;
			}

			b = b->next;
		}

		WVN_ERROR("[HASHMAP|DEBUG] Could not find element matching key.");
		return m_elements[0]->data.second;
	}

	template <typename TKey, typename TValue>
	bool HashMap<TKey, TValue>::contains(const TKey& key)
	{
		Element* b = m_elements[index_of(key)];

		while (b)
		{
			if (b->data.first == key) {
				return true;
			}

			b = b->next;
		}

		return false;
	}

	template <typename TKey, typename TValue>
	int HashMap<TKey, TValue>::element_count() const
	{
		return m_element_count;
	}

	template <typename TKey, typename TValue>
	int HashMap<TKey, TValue>::capacity() const
	{
		return m_capacity;
	}

	template <typename TKey, typename TValue>
	bool HashMap<TKey, TValue>::is_empty() const
	{
		for (int i = 0; i < m_capacity; i++) {
			if (m_elements[i]) {
				return false;
			}
		}

		return true;
	}

	template <typename TKey, typename TValue>
	int HashMap<TKey, TValue>::index_of(const TKey& key) const
	{
		return std::hash<TKey>{}(key) % m_capacity;
	}

	template <typename TKey, typename TValue>
	typename HashMap<TKey, TValue>::Element* HashMap<TKey, TValue>::first()
	{
		for (int i = 0; i < m_capacity; i++) {
			if (m_elements[i]) {
				return m_elements[i];
			}
		}

		return nullptr;
	}

	template <typename TKey, typename TValue>
	const typename HashMap<TKey, TValue>::Element* HashMap<TKey, TValue>::first() const
	{
		for (int i = 0; i < m_capacity; i++) {
			if (m_elements[i]) {
				return m_elements[i];
			}
		}

		return nullptr;
	}

	template <typename TKey, typename TValue>
	typename HashMap<TKey, TValue>::Element* HashMap<TKey, TValue>::last()
	{
		for (int i = m_capacity - 1; i >= 0; i--) {
			if (m_elements[i]) {
				return m_elements[i];
			}
		}

		return nullptr;
	}

	template <typename TKey, typename TValue>
	const typename HashMap<TKey, TValue>::Element* HashMap<TKey, TValue>::last() const
	{
		for (int i = m_capacity; i >= 0; i--) {
			if (m_elements[i]) {
				return m_elements[i];
			}
		}

		return nullptr;
	}

	template <typename TKey, typename TValue>
	typename HashMap<TKey, TValue>::Iterator HashMap<TKey, TValue>::begin()
	{
		return Iterator(first());
	}

	template <typename TKey, typename TValue>
	typename HashMap<TKey, TValue>::ConstIterator HashMap<TKey, TValue>::begin() const
	{
		return ConstIterator(first());
	}

	template <typename TKey, typename TValue>
	typename HashMap<TKey, TValue>::ConstIterator HashMap<TKey, TValue>::cbegin() const
	{
		return ConstIterator(first());
	}

	template <typename TKey, typename TValue>
	typename HashMap<TKey, TValue>::Iterator HashMap<TKey, TValue>::end()
	{
		return Iterator(nullptr);
	}

	template <typename TKey, typename TValue>
	typename HashMap<TKey, TValue>::ConstIterator HashMap<TKey, TValue>::end() const
	{
		return ConstIterator(nullptr);
	}

	template <typename TKey, typename TValue>
	typename HashMap<TKey, TValue>::ConstIterator HashMap<TKey, TValue>::cend() const
	{
		return ConstIterator(nullptr);
	}

	template <typename TKey, typename TValue>
	TValue& HashMap<TKey, TValue>::operator [] (const TKey& idx)
	{
		return get(idx);
	}

	template <typename TKey, typename TValue>
	const TValue& HashMap<TKey, TValue>::operator [] (const TKey& idx) const
	{
		return get(idx);
	}
};

#endif // HASH_MAP_H

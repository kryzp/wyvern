#ifndef DEQUE_H
#define DEQUE_H

#include <wvn/common.h>
#include <new>

// todo: add WVN_ASSERT's

namespace wvn
{
	/**
	 * Dynamically-resizing double ended queue to which you can add
	 * new objects or pick the first one out of the queue.
	 */
	template <typename T, u64 ChunkSize = 64>
	class Deque
	{
		enum
		{
			EXPAND_FRONT,
			EXPAND_BACK
		};

	public:
		class Iterator
		{
			friend class Deque<T, ChunkSize>;

		public:
			Iterator() : m_cur(nullptr), m_first(nullptr), m_last(nullptr), m_chunk(nullptr) { }
			Iterator(T* curr, T* first, T* last, T** chunk) : m_cur(curr), m_first(first), m_last(last), m_chunk(chunk) { }
			Iterator(T** chunk) : m_cur(nullptr), m_first(nullptr), m_last(nullptr), m_chunk(chunk) { set_chunk(chunk); }
			~Iterator() = default;

			T& operator * () const { return *m_cur; }
			T* operator -> () const { return m_cur; }

			T* get() { return m_cur; }
			const T* get() const { return m_cur; }

			void set_chunk(T** chunk) {
				m_chunk = chunk;
				m_first = *chunk;
				m_last = m_first + ChunkSize;
			}

			Iterator& operator ++ () {
				m_cur++;
				if (m_cur == m_last) {
					set_chunk(m_chunk + 1);
					m_cur = m_first;
				}
				return *this;
			}

			Iterator& operator -- () {
				if (m_cur == m_first) {
					set_chunk(m_chunk - 1);
					m_cur = m_last;
				}
				m_cur--;
				return *this;
			}

			Iterator operator ++ (int) {
				Iterator t = *this;
				++(*this);
				return t;
			}

			Iterator operator -- (int) {
				Iterator t = *this;
				--(*this);
				return t;
			}

			Iterator& operator += (s64 d) {
				s64 offset = d + m_cur - m_first;
				if (offset >= 0 && offset < ChunkSize) {
					m_cur += d;
				} else {
					s64 chunk_offset = 0;
					if (offset > 0) {
						chunk_offset = offset / ChunkSize;
					} else {
						chunk_offset = ((offset + 1) / ChunkSize) - 1;
					}
					set_chunk(m_chunk + chunk_offset);
					m_cur = m_first + (offset - (chunk_offset * ChunkSize));
				}
				return *this;
			}

			Iterator& operator -= (s64 d) {
				return (*this) += -d;
			}

			Iterator operator + (s64 d) const {
				Iterator t = *this;
				return t += d;
			}

			Iterator operator - (s64 d) const {
				Iterator t = *this;
				return t -= d;
			}

			T& operator [] (s64 d) const {
				return *((*this) + d);
			}

			bool operator == (const Iterator& other) const { return this->m_cur == other.m_cur && this->m_first == other.m_first && this->m_last == other.m_last && this->m_chunk == other.m_chunk; }
			bool operator != (const Iterator& other) const { return this->m_cur != other.m_cur || this->m_first != other.m_first || this->m_last != other.m_last || this->m_chunk != other.m_chunk; }

		private:
			T* m_cur;
			T* m_first;
			T* m_last;
			T** m_chunk;
		};

		Deque(int initial_capacity = 8);

		Deque(const Deque& other);
		Deque(Deque&& other) noexcept;

		Deque& operator = (const Deque& other);
		Deque& operator = (Deque&& other) noexcept;

		~Deque();

		void clear();
		bool empty() const;
		u64 size() const;
		void swap(Deque& other);

		T& push_back(const T& item);
		T& push_front(const T& item);

		T pop_back();
		T pop_front();

		template <typename... Args>
		T& emplace_front(Args&&... args);

		template <typename... Args>
		T& emplace_back(Args&&... args);

		T& front();
		const T& front() const;
		T& back();
		const T& back() const;

		Iterator begin();
		Iterator end();

		T& at(u64 idx);
		const T& at(u64 idx) const;

		T& operator [] (u64 idx);
		const T& operator [] (u64 idx) const;

	private:
		void increase_size(int front_or_back);
		void expand_front();
		void expand_back();

		constexpr int chunks() const;

		Iterator m_begin;
		Iterator m_end;

		T** m_map;
		u64 m_size;
		u64 m_capacity;
	};

	template <typename T, u64 ChunkSize>
	Deque<T, ChunkSize>::Deque(int initial_capacity)
		: m_begin()
		, m_end()
		, m_map(nullptr)
		, m_size(0)
		, m_capacity(0)
	{
		T** new_map = (T**)::operator new (sizeof(T*) * initial_capacity);
		mem::set(new_map, 0, sizeof(T*) * initial_capacity);

		T** base = new_map + (initial_capacity / 2);

		for (int j = 0; j < initial_capacity; j++) {
			if (!new_map[j]) {
				new_map[j] = (T*)::operator new (sizeof(T) * ChunkSize);
			}
		}

		m_map = new_map;
		m_capacity = initial_capacity * ChunkSize;

		m_begin.set_chunk(base);
		m_begin.m_cur = m_begin.m_first;

		m_end.set_chunk(base);
		m_end.m_cur = m_end.m_first + 1;
	}

	template <typename T, u64 ChunkSize>
	Deque<T, ChunkSize>::Deque(const Deque& other)
		: Deque()
	{
		// todo: use a more efficient approach: just copy all the memory over using mem::copy and then calculate the final m_begin and m_end positions.

		clear();

		for (T* cur = other.m_begin.m_cur + 1; cur < other.m_end.m_cur; cur++) {
			push_back(*cur);
		}
	}

	template <typename T, u64 ChunkSize>
	Deque<T, ChunkSize>::Deque(Deque&& other) noexcept
		: Deque()
	{
		clear();
		swap(other);
	}

	template <typename T, u64 ChunkSize>
	Deque<T, ChunkSize>& Deque<T, ChunkSize>::operator = (const Deque& other)
	{
		// todo: use a more efficient approach: just copy all the memory over using mem::copy and then calculate the final m_begin and m_end positions.

		clear();

		for (T* cur = other.m_begin.m_cur + 1; cur < other.m_end.m_cur; cur++) {
			push_back(*cur);
		}

		return *this;
	}

	template <typename T, u64 ChunkSize>
	Deque<T, ChunkSize>& Deque<T, ChunkSize>::operator = (Deque&& other) noexcept
	{
		clear();
		swap(other);

		return *this;
	}

	template <typename T, u64 ChunkSize>
	Deque<T, ChunkSize>::~Deque()
	{
		clear();

		for (int i = 0; i < chunks(); i++) {
			::operator delete (m_map[i], sizeof(T) * ChunkSize);
		}

		::operator delete (m_map, sizeof(T*) * chunks());

		m_map = nullptr;
		m_capacity = 0;
		m_size = 0;
	}

	template <typename T, u64 ChunkSize>
	void Deque<T, ChunkSize>::clear()
	{
		for (Iterator ptr = m_begin + 1; ptr.m_cur <= m_end.m_cur; ptr++) {
			ptr->~T();
		}

		for (int i = 0; i < chunks(); i++) {
			mem::set(m_map[i], 0, sizeof(T) * ChunkSize);
		}

		m_size = 0;

		T** base  = m_map + (chunks() / 2);

		m_begin.set_chunk(base);
		m_begin.m_cur = m_begin.m_first;

		m_end.set_chunk(base);
		m_end.m_cur = m_end.m_first + 1;
	}

	template <typename T, u64 ChunkSize>
	void Deque<T, ChunkSize>::increase_size(int front_or_back)
	{
		u64 num_chunks = chunks() * 2;

		T** new_map = (T**)::operator new (sizeof(T*) * num_chunks);
		mem::set(new_map, 0, sizeof(T*) * num_chunks);

		int beg_off = m_begin.m_cur   - m_begin.m_first;
		int end_off = m_end  .m_cur   - m_end.m_first;
		int beg_chk = m_begin.m_chunk - m_map;
		int end_chk = m_end  .m_chunk - m_begin.m_chunk;

		if (front_or_back == EXPAND_FRONT) {
			mem::copy(new_map + num_chunks - chunks(), m_map, sizeof(T*) * chunks());
		} else if (front_or_back == EXPAND_BACK) {
			mem::copy(new_map, m_map, sizeof(T*) * chunks());
		}

		// todo: based on front_or_back, this will only need to go half way through the new_map, starting at 0 or chunks(), so the speed of this loop could be halved, but im lazy right now
		// should only be like 2 lines of code :/
		// L
		for (int j = 0; j < num_chunks; j++) {
			if (!new_map[j]) {
				new_map[j] = (T*)::operator new (sizeof(T) * ChunkSize);
			}
		}

		if (front_or_back == EXPAND_FRONT) {
			m_begin.set_chunk(new_map + num_chunks - chunks());
			m_end  .set_chunk(new_map + num_chunks - chunks() + end_chk);
		} else if (front_or_back == EXPAND_BACK) {
			m_begin.set_chunk(new_map + beg_chk  + 1);
			m_end  .set_chunk(new_map + chunks() - 1);
		}

		m_begin.m_cur = m_begin.m_first + beg_off;
		m_end  .m_cur = m_end  .m_first + end_off;

		::operator delete (m_map, sizeof(T*) * m_capacity);

		m_map = new_map;
		m_capacity = num_chunks * ChunkSize;
	}

	template <typename T, u64 ChunkSize>
	void Deque<T, ChunkSize>::expand_front()
	{
		if ((m_begin - 1).m_chunk <= m_map) {
			increase_size(EXPAND_FRONT);
		}
		m_size++;
	}

	template <typename T, u64 ChunkSize>
	void Deque<T, ChunkSize>::expand_back()
	{
		if ((m_end + 1).m_chunk >= (m_map + chunks())) {
			increase_size(EXPAND_BACK);
		}
		m_size++;
	}

	template <typename T, u64 ChunkSize>
	constexpr int Deque<T, ChunkSize>::chunks() const
	{
		return m_capacity / ChunkSize;
	}

	template <typename T, u64 ChunkSize>
	bool Deque<T, ChunkSize>::empty() const
	{
		return m_size == 0;
	}

	template <typename T, u64 ChunkSize>
	u64 Deque<T, ChunkSize>::size() const
	{
		return m_size;
	}

	template <typename T, u64 ChunkSize>
	void Deque<T, ChunkSize>::swap(Deque& other)
	{
		Iterator t_begin = this->m_begin;
		Iterator t_end = this->m_end;
		T** t_map = this->m_map;
		u64 t_size = this->m_size;
		u64 t_capacity = this->m_capacity;

		this->m_begin = other.m_begin;
		this->m_end = other.m_end;
		this->m_map = other.m_map;
		this->m_size = other.m_size;
		this->m_capacity = other.m_capacity;

		other.m_begin = t_begin;
		other.m_end = t_end;
		other.m_map = t_map;
		other.m_size = t_size;
		other.m_capacity = t_capacity;
	}

	template <typename T, u64 ChunkSize>
	T& Deque<T, ChunkSize>::push_front(const T& item)
	{
		expand_front();
		(*m_begin) = std::move(item);
		m_begin--;
		return *(m_begin + 1);
	}

	template <typename T, u64 ChunkSize>
	T& Deque<T, ChunkSize>::push_back(const T& item)
	{
		expand_back();
		(*m_end) = std::move(item);
		m_end++;
		return *(m_end - 1);
	}

	template <typename T, u64 ChunkSize>
	T Deque<T, ChunkSize>::pop_front()
	{
		m_size--;
		m_begin++;
		return *m_begin;
	}

	template <typename T, u64 ChunkSize>
	T Deque<T, ChunkSize>::pop_back()
	{
		m_size--;
		m_end--;
		return *m_end;
	}

	template <typename T, u64 ChunkSize>
	template <typename... Args>
	T& Deque<T, ChunkSize>::emplace_front(Args&&... args)
	{
		expand_front();
		new (m_begin.get()) T(std::forward<Args>(args)...);
		m_begin--;
		return *(m_begin + 1);
	}

	template <typename T, u64 ChunkSize>
	template <typename... Args>
	T& Deque<T, ChunkSize>::emplace_back(Args&&... args)
	{
		expand_back();
		new (m_end.get()) T(std::forward<Args>(args)...);
		m_end++;
		return *(m_end - 1);
	}

	template <typename T, u64 ChunkSize>
	T& Deque<T, ChunkSize>::front()
	{
		return *(m_begin + 1);
	}

	template <typename T, u64 ChunkSize>
	const T& Deque<T, ChunkSize>::front() const
	{
		return *(m_begin + 1);
	}

	template <typename T, u64 ChunkSize>
	T& Deque<T, ChunkSize>::back()
	{
		return *(m_end - 1);
	}

	template <typename T, u64 ChunkSize>
	const T& Deque<T, ChunkSize>::back() const
	{
		return *(m_end - 1);
	}

	template <typename T, u64 ChunkSize>
	typename Deque<T, ChunkSize>::Iterator Deque<T, ChunkSize>::begin()
	{
		return m_begin;
	}

	template <typename T, u64 ChunkSize>
	typename Deque<T, ChunkSize>::Iterator Deque<T, ChunkSize>::end()
	{
		return m_end;
	}

	template <typename T, u64 ChunkSize>
	T& Deque<T, ChunkSize>::at(u64 idx)
	{
		return m_begin[idx];
	}

	template <typename T, u64 ChunkSize>
	const T& Deque<T, ChunkSize>::at(u64 idx) const
	{
		return m_begin[idx];
	}

	template <typename T, u64 ChunkSize>
	T& Deque<T, ChunkSize>::operator [] (u64 idx)
	{
		return m_begin[idx];
	}

	template <typename T, u64 ChunkSize>
	const T& Deque<T, ChunkSize>::operator [] (u64 idx) const
	{
		return m_begin[idx];
	}
}

#endif // DEQUE_H

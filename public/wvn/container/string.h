#ifndef STRING_H_
#define STRING_H_

#include <wvn/common.h>
#include <wvn/container/vector.h>

namespace wvn
{
	/**
	 * Generic-sized C-style string wrapper.
	 */
	template <u64 Size>
	class Str
	{
	public:
		class Iterator
		{
			friend class Str<Size>;

		public:
			Iterator() : m_char(nullptr) { }
			Iterator(char* ptr) : m_char(ptr) { }
			~Iterator() = default;
			char& operator * () const { return *m_char; }
			char* operator -> () const { return &(*m_char); }
			Iterator& operator ++ () { m_char++; return *this; }
			Iterator& operator -- () { m_char--; return *this; }
			Iterator& operator ++ (int) { m_char++; return *this; }
			Iterator& operator -- (int) { m_char--; return *this; }
			bool operator == (const Iterator& other) const { return this->m_char == other.m_char; }
			bool operator != (const Iterator& other) const { return this->m_char != other.m_char; }
		private:
			char* m_char;
		};

		class ConstIterator
		{
			friend class Str<Size>;

		public:
			ConstIterator() : m_char(nullptr) { }
			ConstIterator(const char* ptr) : m_char(ptr) { }
			~ConstIterator() = default;
			const char& operator * () const { return *m_char; }
			const char* operator -> () const { return &(*m_char); }
			ConstIterator& operator ++ () { m_char++; return *this; }
			ConstIterator& operator -- () { m_char--; return *this; }
			ConstIterator& operator ++ (int) { m_char++; return *this; }
			ConstIterator& operator -- (int) { m_char--; return *this; }
			bool operator == (const ConstIterator& other) const { return this->m_char == other.m_char; }
			bool operator != (const ConstIterator& other) const { return this->m_char != other.m_char; }
		private:
			const char* m_char;
		};

		class ReverseIterator
		{
			friend class Str<Size>;

		public:
			ReverseIterator() : m_char(nullptr) { }
			ReverseIterator(char* ptr) : m_char(ptr) { }
			~ReverseIterator() = default;
			char& operator * () const { return *m_char; }
			char* operator -> () const { return &(*m_char); }
			ReverseIterator& operator ++ () { m_char--; return *this; }
			ReverseIterator& operator -- () { m_char++; return *this; }
			ReverseIterator& operator ++ (int) { m_char--; return *this; }
			ReverseIterator& operator -- (int) { m_char++; return *this; }
			bool operator == (const ReverseIterator& other) const { return this->m_char == other.m_char; }
			bool operator != (const ReverseIterator& other) const { return this->m_char != other.m_char; }
		private:
			char* m_char;
		};

		class ReverseConstIterator
		{
			friend class Str<Size>;

		public:
			ReverseConstIterator() : m_char(nullptr) { }
			ReverseConstIterator(const char* ptr) : m_char(ptr) { }
			~ReverseConstIterator() = default;
			const char& operator * () const { return *m_char; }
			const char* operator -> () const { return &(*m_char); }
			ReverseConstIterator& operator ++ () { m_char--; return *this; }
			ReverseConstIterator& operator -- () { m_char++; return *this; }
			ReverseConstIterator& operator ++ (int) { m_char--; return *this; }
			ReverseConstIterator& operator -- (int) { m_char++; return *this; }
			bool operator == (const ReverseConstIterator& other) const { return this->m_char == other.m_char; }
			bool operator != (const ReverseConstIterator& other) const { return this->m_char != other.m_char; }
		private:
			const char* m_char;
		};

		Str();
		Str(const char* str);

		Str(const Str& other);
		Str(Str&& other) noexcept;

		Str& operator = (const Str& other);
		Str& operator = (Str&& other) noexcept;

		~Str();

		void clear();
		bool empty() const;

		Str& append(const Str& str);

		Str to_upper() const;
		Str to_lower() const;

		Str trim() const;
		Str trim_start() const;
		Str trim_end() const;

		Str strip_newline() const;

		Vector<Str> split(const char* delimiter = " ") const;

		int index_of(const Str& str) const;
		int last_index_of(const Str& str) const;

		bool starts_with(const Str& str) const;
		bool ends_with(const Str& str) const;
		bool contains(const Str& str) const;

		void push_front(char c);
		void push_back(char c);

		void pop_front();
		void pop_back();

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
		ReverseConstIterator rcbegin() const;
		ReverseConstIterator rcend() const;

		char* c_str();
		const char* c_str() const;

		u64 length() const;
		constexpr u64 size() const;

		char* at(u64 idx);
		const char* at(u64 idx) const;

		char& operator [] (u64 idx);
		const char& operator [] (u64 idx) const;

		bool operator == (const Str& other) const;
		bool operator != (const Str& other) const;

		Str operator + (const Str& other) const;
		Str& operator += (const Str& other);

		explicit operator char* ();
		explicit operator const char* () const;

	private:
		char* m_buf;
		u64 m_length;
	};

	using String = Str<512>;

	template <u64 Size>
	Str<Size>::Str()
		: m_length(0)
	{
		m_buf = new char[Size];
		mem::set(m_buf, 0, Size);
	}

	template <u64 Size>
	Str<Size>::Str(const char* str)
		: m_length(cstr::length(str))
	{
		wvn_ASSERT(m_length < (Size - 1), "[STRING|DEBUG] Length must not exceed maximum size."); // -1 for '\0'

		m_buf = new char[Size];
		cstr::copy(m_buf, str, m_length);
		m_buf[m_length] = '\0';
	}
	
	template <u64 Size>
	Str<Size>::Str(const Str& other)
	{
		wvn_ASSERT(other.m_length < (Size - 1), "[STRING|DEBUG] Length must not exceed maximum size.");

		m_buf = new char[Size];

		m_length = other.m_length;
		cstr::copy(m_buf, other.m_buf, other.m_length);

		m_buf[m_length] = '\0';
	}
	
	template <u64 Size>
	Str<Size>::Str(Str&& other) noexcept
	{
		wvn_ASSERT(other.m_length < (Size - 1), "[STRING|DEBUG] Length must not exceed maximum size.");

		m_buf = new char[Size];

		m_length = std::move(other.m_length);
		m_buf = std::move(other.m_buf);

		other.m_length = 0;
		other.m_buf = nullptr;
	}
	
	template <u64 Size>
	Str<Size>& Str<Size>::operator = (const Str& other)
	{
		wvn_ASSERT(other.m_length < (Size - 1), "[STRING|DEBUG] Length must not exceed maximum size.");

		if (!m_buf)
			m_buf = new char[Size];
		
		if (m_length > other.m_length)
			mem::set(m_buf, 0, m_length);

		m_length = other.m_length;
		cstr::copy(m_buf, other.m_buf, other.m_length);
		m_buf[m_length] = '\0';

		return *this;
	}
	
	template <u64 Size>
	Str<Size>& Str<Size>::operator = (Str&& other) noexcept
	{
		wvn_ASSERT(other.m_length < (Size - 1), "[STRING|DEBUG] Length must not exceed maximum size");

		if (!m_buf)
			m_buf = new char[Size];

		m_length = std::move(other.m_length);
		m_buf = std::move(other.m_buf);

		other.m_length = 0;
		other.m_buf = nullptr;

		return *this;
	}

	template <u64 Size>
	Str<Size>::~Str()
	{
		m_length = 0;
		delete[] m_buf;
	}

	template <u64 Size>
	void Str<Size>::clear()
	{
		mem::set(m_buf, 0, m_length);
		m_length = 0;
	}

	template <u64 Size>
	bool Str<Size>::empty() const
	{
		return cstr::compare(m_buf, "") == 0;
	}

	template <u64 Size>
	Str<Size>& Str<Size>::append(const Str<Size>& str)
	{
		wvn_ASSERT((m_length + str.m_length) < (Size - 1), "[STRING|DEBUG] Final length must not exceed maximum size.");

		m_length += str.length();
		cstr::concat(m_buf, str.m_buf, str.length());

		return *this;
	}

	template <u64 Size>
	void Str<Size>::push_front(char c)
	{
		mem::move(m_buf + 1, m_buf, m_length);
		m_buf[0] = c;
		m_length++;
	}

	template <u64 Size>
	void Str<Size>::push_back(char c)
	{
		m_buf[m_length] = c;
		m_length++;
		m_buf[m_length] = '\0';
	}

	template <u64 Size>
	void Str<Size>::pop_front()
	{
		m_length--;
		mem::move(m_buf, m_buf + 1, m_length);
	}

	template <u64 Size>
	void Str<Size>::pop_back()
	{
		m_length--;
		m_buf[m_length] = '\0';
	}

	template <u64 Size>
	typename Str<Size>::Iterator Str<Size>::begin()
	{
		return Iterator(m_buf);
	}

	template <u64 Size>
	typename Str<Size>::ConstIterator Str<Size>::begin() const
	{
		return ConstIterator(m_buf);
	}

	template <u64 Size>
	typename Str<Size>::Iterator Str<Size>::end()
	{
		return Iterator(m_buf + m_length);
	}

	template <u64 Size>
	typename Str<Size>::ConstIterator Str<Size>::end() const
	{
		return ConstIterator(m_buf + m_length);
	}

	template <u64 Size>
	typename Str<Size>::ReverseIterator Str<Size>::rbegin()
	{
		return ReverseIterator(m_buf + m_length - 1);
	}

	template <u64 Size>
	typename Str<Size>::ReverseConstIterator Str<Size>::rbegin() const
	{
		return ReverseConstIterator(m_buf + m_length - 1);
	}

	template <u64 Size>
	typename Str<Size>::ReverseIterator Str<Size>::rend()
	{
		return ReverseIterator(m_buf - 1);
	}

	template <u64 Size>
	typename Str<Size>::ReverseConstIterator Str<Size>::rend() const
	{
		return ReverseConstIterator(m_buf - 1);
	}

	template <u64 Size>
	typename Str<Size>::ConstIterator Str<Size>::cbegin() const
	{
		return ConstIterator(m_buf);
	}

	template <u64 Size>
	typename Str<Size>::ConstIterator Str<Size>::cend() const
	{
		return ConstIterator(m_buf + m_length);
	}

	template <u64 Size>
	typename Str<Size>::ReverseConstIterator Str<Size>::rcbegin() const
	{
		return ReverseConstIterator(m_buf + m_length - 1);
	}

	template <u64 Size>
	typename Str<Size>::ReverseConstIterator Str<Size>::rcend() const
	{
		return ReverseConstIterator(m_buf - 1);
	}

	template <u64 Size>
	char* Str<Size>::c_str()
	{
		return m_buf;
	}

	template <u64 Size>
	const char* Str<Size>::c_str() const
	{
		return m_buf;
	}

	template <u64 Size>
	u64 Str<Size>::length() const
	{
		return m_length;
	}

	template <u64 Size>
	constexpr u64 Str<Size>::size() const
	{
		return Size;
	}

	template <u64 Size>
	Str<Size> Str<Size>::trim() const
	{
		return trim_start().trim_end();
	}

	template <u64 Size>
	Str<Size> Str<Size>::trim_start() const
	{
		const char* buffer = m_buf;
		while (cstr::is_space(*buffer)) buffer++;
		int whitespace = buffer - m_buf;

		Str trimmed = *this;
		trimmed.m_length -= whitespace;
		mem::move(trimmed.m_buf, trimmed.m_buf + whitespace, trimmed.m_length);

		return trimmed;
	}

	template <u64 Size>
	Str<Size> Str<Size>::trim_end() const
	{
		const char* end = m_buf + m_length - 1;

		const char* buffer = end;
		while (buffer > m_buf && cstr::is_space(*buffer)) buffer--;
		int whitespace = buffer - end;

		Str<Size> trimmed = *this;
		trimmed.m_length -= whitespace;
		trimmed.m_buf[trimmed.m_length] = '\0';

		return trimmed.strip_newline();
	}

	template <u64 Size>
	Str<Size> Str<Size>::strip_newline() const
	{
		Str<Size> cpy = *this;
		cpy.m_buf[cstr::cspan(cpy.m_buf, "\n")] = 0;
		cpy.m_buf[cstr::cspan(cpy.m_buf, "\r\n")] = 0;
		return cpy;
	}

	template <u64 Size>
	Vector<Str<Size>> Str<Size>::split(const char* delimiter) const
	{
		Vector<Str<Size>> tokens;

		char tokenbuf[Size] = { 0 };
		mem::copy(tokenbuf, m_buf, m_length);

		char* token = cstr::token(tokenbuf, delimiter);

		while (token)
		{
			tokens.push_back(token);
			token = cstr::token(nullptr, delimiter);
		}

		return tokens;
	}

	template <u64 Size>
	Str<Size> Str<Size>::to_upper() const
	{
		Str<Size> copy = *this;

		for (auto& c : copy)
			c = cstr::to_upper(c);

		return copy;
	}

	template <u64 Size>
	Str<Size> Str<Size>::to_lower() const
	{
		Str<Size> copy = *this;

		for (auto& c : copy)
			c = cstr::to_lower(c);

		return copy;
	}

	template <u64 Size>
	int Str<Size>::index_of(const Str& str) const
	{
		wvn_ASSERT(m_length >= str.m_length, "[STRING|DEBUG] String to check for must not be larger than the string getting checked.");

		for (unsigned i = 0; i <= m_length - str.m_length; i++)
		{
			unsigned j = 0;

			while (j < str.m_length && m_buf[i+j] == str[j]) {
				j++;
			}

			if (j == str.m_length) {
				return static_cast<int>(i);
			}
		}

		return -1;
	}

	template <u64 Size>
	int Str<Size>::last_index_of(const Str<Size>& str) const
	{
		wvn_ASSERT(m_length >= str.m_length, "[STRING|DEBUG] String to check for must not be larger than the string getting checked.");

		for (unsigned i = m_length - str.m_length; i > 0; i--)
		{
			unsigned j = 0;

			while (j < str.m_length && m_buf[i+j] == str[j]) {
				j++;
			}

			if (j == str.m_length) {
				return static_cast<int>(i);
			}
		}

		return -1;
	}

	template <u64 Size>
	bool Str<Size>::starts_with(const Str& str) const
	{
		for (int i = 0; i < str.length(); i++)
		{
			if (m_buf[i] != str[i])
				return false;
		}

		return true;
	}

	template <u64 Size>
	bool Str<Size>::ends_with(const Str& str) const
	{
		for (int i = 0; i < str.length(); i++)
		{
			if (m_buf[m_length - str.length() + i] != str[i])
				return false;
		}

		return true;
	}

	template <u64 Size>
	bool Str<Size>::contains(const Str& str) const
	{
		return index_of(str) != -1;
	}

	template <u64 Size>
	char* Str<Size>::at(u64 idx)
	{
		wvn_ASSERT(idx < m_length, "[STRING|DEBUG] Index must not be more than the length of the string.");
		return m_buf[idx];
	}

	template <u64 Size>
	const char* Str<Size>::at(u64 idx) const
	{
		wvn_ASSERT(idx < m_length, "[STRING|DEBUG] Index must not be more than the length of the string.");
		return m_buf[idx];
	}

	template <u64 Size>
	char& Str<Size>::operator [] (u64 idx)
	{
		wvn_ASSERT(idx < m_length, "[STRING|DEBUG] Index must not be more than the length of the string.");
		return m_buf[idx];
	}

	template <u64 Size>
	const char& Str<Size>::operator [] (u64 idx) const
	{
		wvn_ASSERT(idx < m_length, "[STRING|DEBUG] Index must not be more than the length of the string.");
		return m_buf[idx];
	}

	template <u64 Size>
	Str<Size> Str<Size>::operator + (const Str<Size>& other) const
	{
		Str str = *this;
		str.append(other);
		return str;
	}

	template <u64 Size>
	Str<Size>& Str<Size>::operator += (const Str<Size>& other)
	{
		return append(other);
	}

	template <u64 Size>
	bool Str<Size>::operator == (const Str& other) const
	{
		if (!other.m_buf || !this->m_buf)
			return false;

		return cstr::compare(m_buf, other.m_buf) == 0;
	}

	template <u64 Size>
	bool Str<Size>::operator != (const Str& other) const
	{
		return !(*this == other);
	}

	template <u64 Size>
	Str<Size>::operator const char* () const
	{
		return m_buf;
	}

	template <u64 Size>
	Str<Size>::operator char* ()
	{
		return m_buf;
	}
}

#endif // STRING_H_

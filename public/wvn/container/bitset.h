#ifndef BITSET_H_
#define BITSET_H_

#include <wvn/common.h>

namespace wvn
{
	/**
	 * A wrapper around a memory efficient array of booleans that
	 * can be toggled is_on and is_off.
	 */
	template <u64 Size>
	class Bitset
	{
	public:
		Bitset();
		~Bitset();

		bool all() const;
		bool none() const;
		bool any() const;

		u64 on_count() const;
		u64 off_count() const;

		Bitset& reset();
		Bitset& invert();

		Bitset& enable(u64 idx);
		Bitset& disable(u64 idx);
		Bitset& set(u64 idx, bool mode);
		Bitset& toggle(u64 idx);

		bool is_on(u64 idx) const;
		bool is_on_only(u64 idx) const;
		bool is_off(u64 idx) const;

		bool operator [] (u64 idx) const;
		
		constexpr inline u64 size() const;
		constexpr inline u64 memory_size() const;

	private:
		b8* m_bytes;
	};

	template <u64 Size>
	Bitset<Size>::Bitset()
	{
		m_bytes = new b8[memory_size()];
		reset();
	}

	template <u64 Size>
	Bitset<Size>::~Bitset()
	{
		delete[] m_bytes;
	}

	template <u64 Size>
	bool Bitset<Size>::all() const
	{
		for (u64 i = 0; i < Size; i++)
		{
			if (is_off(i))
				return false;
		}

		return true;
	}

	template <u64 Size>
	bool Bitset<Size>::none() const
	{
		for (u64 i = 0; i < Size; i++)
		{
			if (is_on(i))
				return false;
		}

		return true;
	}

	template <u64 Size>
	bool Bitset<Size>::any() const
	{
		return !none();
	}

	template <u64 Size>
	u64 Bitset<Size>::on_count() const
	{
		// popcount???

		u64 total = 0;
		for (u64 i = 0; i < Size; i++)
		{
			if (is_on(i))
				total++;
		}

		return total;
	}

	template <u64 Size>
	u64 Bitset<Size>::off_count() const
	{
		return Size - on_count();
	}

	template <u64 Size>
	Bitset<Size>& Bitset<Size>::reset()
	{
		mem::set(m_bytes, 0, memory_size());
		return *this;
	}

	template <u64 Size>
	Bitset<Size>& Bitset<Size>::invert()
	{
		for (int i = 0; i < Size; i++) toggle(i);
		return *this;
	}

	template <u64 Size>
	Bitset<Size>& Bitset<Size>::enable(u64 idx)
	{
		wvn_ASSERT(idx >= 0 && idx < Size, "[BITSET|DEBUG] Index must be within range of the bitset.");
		m_bytes[idx/8] |= (1 << idx);
		return *this;
	}

	template <u64 Size>
	Bitset<Size>& Bitset<Size>::disable(u64 idx)
	{
		wvn_ASSERT(idx >= 0 && idx < Size, "[BITSET|DEBUG] Index must be within range of the bitset.");
		m_bytes[idx/8] &= ~(1 << idx);
		return *this;
	}

	template <u64 Size>
	Bitset<Size>& Bitset<Size>::toggle(u64 idx)
	{
		wvn_ASSERT(idx >= 0 && idx < Size, "[BITSET|DEBUG] Index must be within range of the bitset.");
		m_bytes[idx/8] ^= 1 << idx;
		return *this;
	}

	template <u64 Size>
	Bitset<Size>& Bitset<Size>::set(u64 idx, bool mode)
	{
		wvn_ASSERT(idx >= 0 && idx < Size, "[BITSET|DEBUG] Index must be within range of the bitset.");
		if (mode) m_bytes[idx/8] |= (1 << idx);
		else m_bytes[idx/8] &= ~(1 << idx);
		return *this;
	}

	template <u64 Size>
	bool Bitset<Size>::is_on(u64 idx) const
	{
		wvn_ASSERT(idx >= 0 && idx < Size, "[BITSET|DEBUG] Index must be within range of the bitset.");
		return (m_bytes[idx/8] & (1 << idx)) != 0;
	}

	template <u64 Size>
	bool Bitset<Size>::is_on_only(u64 idx) const
	{
		wvn_ASSERT(idx >= 0 && idx < Size, "[BITSET|DEBUG] Index must be within range of the bitset.");
		return (m_bytes[idx/8] & (1 << idx)) == (1 << idx);
	}

	template <u64 Size>
	bool Bitset<Size>::is_off(u64 idx) const
	{
		return !is_on(idx);
	}

	template <u64 Size>
	bool Bitset<Size>::operator [] (u64 idx) const
	{
		return is_on(idx);
	}

	template <u64 Size>
	constexpr inline u64 Bitset<Size>::size() const
	{
		return Size;
	}

	template <u64 Size>
	constexpr inline u64 Bitset<Size>::memory_size() const
	{
		return (Size / 8) + 1;
	}
}

#endif // BITSET_H_

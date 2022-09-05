#pragma once

#include <wvn/util/types.h>

namespace wvn
{
	template <u64 TSize>
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

		bool on(u64 idx) const;
		bool on_only(u64 idx) const;
		bool off(u64 idx) const;

		bool operator [] (u64 idx) const;
		
		constexpr inline u64 size() const;
		constexpr inline u64 memory_size() const;

	private:
		u8* m_bits;
	};

	template <u64 TSize>
	Bitset<TSize>::Bitset()
	{
		m_bits = new u8[memory_size()];
		reset();
	}

	template <u64 TSize>
	Bitset<TSize>::~Bitset()
	{
		delete[] m_bits;
	}

	template <u64 TSize>
	bool Bitset<TSize>::all() const
	{
		for (u64 i = 0; i < TSize; i++)
		{
			if (off(i))
				return false;
		}

		return true;
	}

	template <u64 TSize>
	bool Bitset<TSize>::none() const
	{
		for (u64 i = 0; i < TSize; i++)
		{
			if (on(i))
				return false;
		}

		return true;
	}

	template <u64 TSize>
	bool Bitset<TSize>::any() const
	{
		return !none();
	}

	template <u64 TSize>
	u64 Bitset<TSize>::on_count() const
	{
		// popcount???

		u64 total = 0;
		for (u64 i = 0; i < TSize; i++)
		{
			if (on(i))
				total++;
		}

		return total;
	}

	template <u64 TSize>
	u64 Bitset<TSize>::off_count() const
	{
		return TSize - on_count();
	}

	template <u64 TSize>
	Bitset<TSize>& Bitset<TSize>::reset()
	{
		mem::set(m_bits, 0, memory_size());
		return *this;
	}

	template <u64 TSize>
	Bitset<TSize>& Bitset<TSize>::invert()
	{
		for (int i = 0; i < TSize; i++) toggle(i);
		return *this;
	}

	template <u64 TSize>
	Bitset<TSize>& Bitset<TSize>::enable(u64 idx)
	{
		LEV_ASSERT(idx >= 0 && idx < TSize, "Index must be within range of the bitset");
		m_bits[idx/8] |= (1 << idx);
		return *this;
	}

	template <u64 TSize>
	Bitset<TSize>& Bitset<TSize>::disable(u64 idx)
	{
		LEV_ASSERT(idx >= 0 && idx < TSize, "Index must be within range of the bitset");
		m_bits[idx/8] &= ~(1 << idx);
		return *this;
	}

	template <u64 TSize>
	Bitset<TSize>& Bitset<TSize>::toggle(u64 idx)
	{
		LEV_ASSERT(idx >= 0 && idx < TSize, "Index must be within range of the bitset");
		m_bits[idx/8] ^= (1 << idx);
		return *this;
	}

	template <u64 TSize>
	Bitset<TSize>& Bitset<TSize>::set(u64 idx, bool mode)
	{
		LEV_ASSERT(idx >= 0 && idx < TSize, "Index must be within range of the bitset");
		if (mode) m_bits[idx/8] |= (1 << idx);
		else m_bits[idx/8] &= ~(1 << idx);
		return *this;
	}

	template <u64 TSize>
	bool Bitset<TSize>::on(u64 idx) const
	{
		LEV_ASSERT(idx >= 0 && idx < TSize, "Index must be within range of the bitset");
		return (m_bits[idx/8] & (1 << idx)) != 0;
	}

	template <u64 TSize>
	bool Bitset<TSize>::on_only(u64 idx) const
	{
		LEV_ASSERT(idx >= 0 && idx < TSize, "Index must be within range of the bitset");
		return (m_bits[idx/8] & (1 << idx)) == idx;
	}

	template <u64 TSize>
	bool Bitset<TSize>::off(u64 idx) const
	{
		return !on(idx);
	}

	template <u64 TSize>
	bool Bitset<TSize>::operator [] (u64 idx) const
	{
		return on(idx);
	}

	template <u64 TSize>
	constexpr inline u64 Bitset<TSize>::size() const
	{
		return TSize;
	}

	template <u64 TSize>
	constexpr inline u64 Bitset<TSize>::memory_size() const
	{
		return (TSize / 8) + 1;
	}
}

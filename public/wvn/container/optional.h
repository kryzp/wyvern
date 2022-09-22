#pragma once

namespace wvn
{
	template <typename T>
	class Optional
	{
	public:
		Optional();
		Optional(const T& value);
		Optional(const T& value, bool enabled);

		Optional(const Optional& other);
		Optional(Optional&& other) noexcept;

		Optional& operator = (const Optional& other);
		Optional& operator = (Optional&& other) noexcept;

		~Optional();

		T& value_or(T& other);
		const T& value_or(const T& other) const;

		T& value();
		const T& value() const;

		constexpr bool has_value() const;

	private:
		T m_value;
		bool m_enabled;
	};

	template <typename T>
	Optional<T>::Optional()
		: m_value()
		, m_enabled(false)
	{
	}

	template <typename T>
	Optional<T>::Optional(const T& value)
		: m_value(value)
		, m_enabled(true)
	{
	}

	template <typename T>
	Optional<T>::Optional(const T& value, bool enabled)
		: m_value(value)
		, m_enabled(enabled)
	{
	}

	template <typename T>
	Optional<T>::Optional(const Optional& other)
		: m_value(other.m_value)
		, m_enabled(other.m_enabled)
	{
	}

	template <typename T>
	Optional<T>::Optional(Optional&& other) noexcept
		: m_value(std::move(other.m_value))
		, m_enabled(std::move(other.m_enabled))
	{
		other.m_enabled = false;
	}

	template <typename T>
	Optional<T>& Optional<T>::operator = (const Optional<T>& other)
	{
		this->m_value = other.m_value;
		this->m_enabled = other.m_enabled;

		return *this;
	}

	template <typename T>
	Optional<T>& Optional<T>::operator = (Optional<T>&& other) noexcept
	{
		this->m_value = std::move(other.m_value);
		this->m_enabled = std::move(other.m_enabled);

		other.m_enabled = false;

		return *this;
	}

	template <typename T>
	Optional<T>::~Optional()
	{
		this->m_enabled = false;
	}

	template <typename T>
	T& Optional<T>::value_or(T& other)
	{
		if (m_enabled) {
			return m_value;
		}

		return other;
	}

	template <typename T>
	const T& Optional<T>::value_or(const T& other) const
	{
		if (m_enabled) {
			return m_value;
		}

		return other;
	}

	template <typename T>
	T& Optional<T>::value()
	{
		return m_value;
	}

	template <typename T>
	const T& Optional<T>::value() const
	{
		return m_value;
	}

	template <typename T>
	constexpr bool Optional<T>::has_value() const
	{
		return m_enabled;
	}
}

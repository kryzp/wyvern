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

		T& value_or(const T& other);

		T value;
		bool enabled;
	};

	template <typename T>
	Optional<T>::Optional()
		: value()
		, enabled(false)
	{
	}

	template <typename T>
	Optional<T>::Optional(const T& value)
		: value(value)
		, enabled(true)
	{
	}

	template <typename T>
	Optional<T>::Optional(const T& value, bool enabled)
		: value(value)
		, enabled(enabled)
	{
	}

	template <typename T>
	T& Optional<T>::value_or(const T& other)
	{
		if (enabled) return value;
		else return other;
	}
}

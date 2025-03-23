#ifndef ANY_H_
#define ANY_H_

namespace wvn
{
	// todo

	/**
	 * Wrapper around a raw pointer to any type possible.
	 */
	class Any
	{
	public:
		Any();
		~Any();

		template <typename T>
		T as();

		void reset();
		bool has_value();

	private:
		void* m_data;
	};

	Any::Any()
	{
	}

	Any::~Any()
	{
	}

	template <typename T>
	T Any::as()
	{
		return T();
	}

	void Any::reset()
	{
	}

	bool Any::has_value()
	{
		return false;
	}
}

#endif // ANY_H_

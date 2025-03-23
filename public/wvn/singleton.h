#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <wvn/common.h>

/**
 * All singletons must also use these two macros
 * to define and implement themselves as just inheriting
 * from Singleton<T> won't do the trick is_on its own.
 */

#define wvn_DEF_SINGLETON(_class) \
public: \
	static _class* get_singleton(); \
private:

#define wvn_IMPL_SINGLETON(_class) \
	template <> _class* Singleton<_class>::ps_singleton = nullptr; \
	_class* _class::get_singleton() { return ps_singleton; }

namespace wvn
{
	/**
	 * Singleton class to be inherited by all types that wish
	 * to be a singleton.
	 */
	template <class T>
	class Singleton
	{
	public:
		Singleton()
		{
			wvn_ASSERT(!ps_singleton, "[SINGLETON|DEBUG] ms_singleton must be nullptr.");
			ps_singleton = static_cast<T*>(this);
		}

		~Singleton()
		{
			wvn_ASSERT(ps_singleton, "[SINGLETON|DEBUG] ms_singleton must not be nullptr.");
			ps_singleton = nullptr;
		}

		static T* get_singleton()
		{
			return ps_singleton;
		}

	protected:
		static T* ps_singleton;
	};
}

#endif // SINGLETON_H_

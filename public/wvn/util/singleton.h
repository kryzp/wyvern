#ifndef SINGLETON_H
#define SINGLETON_H

#include <wvn/util/assert.h>

/**
 * All singletons must also use these two macros
 * to define and implement themselves as just inheriting
 * from Singleton<T> won't do the trick on its own.
 */

#define WVN_DEF_SINGLETON(_class) \
public: \
	static _class* get_singleton(); \
private:

#define WVN_IMPL_SINGLETON(_class) \
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
			WVN_ASSERT(!ps_singleton, "[SINGLETON|DEBUG] ms_singleton must be nullptr.");
			ps_singleton = static_cast<T*>(this);
		}

		~Singleton()
		{
			WVN_ASSERT(ps_singleton, "[SINGLETON|DEBUG] ms_singleton must not be nullptr.");
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

#endif // SINGLETON_H

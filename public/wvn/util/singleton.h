#pragma once

#include <wvn/util/assert.h>

#define WVN_DEF_SINGLETON(_class) \
public: \
	static _class& get_singleton(); \
	static _class* get_singleton_ptr(); \
private:

#define WVN_IMPL_SINGLETON(_class) \
	template <> _class* Singleton<_class>::ps_singleton = nullptr; \
	_class& _class::get_singleton() { WVN_ASSERT(ps_singleton, "ps_singleton must not be nullptr"); return *ps_singleton; } \
	_class* _class::get_singleton_ptr() { return ps_singleton; }

namespace wvn
{
	/*
	 * Singleton class to be inherited by all types that wish
	 * to be a singleton.
	 */
	template <class T>
	class Singleton
	{
	public:
		Singleton()
		{
			WVN_ASSERT(!ps_singleton, "ms_singleton must be nullptr");
			ps_singleton = static_cast<T*>(this);
		}

		~Singleton()
		{
			WVN_ASSERT(ps_singleton, "ms_singleton must not be nullptr");
			ps_singleton = nullptr;
		}

		static T& get_singleton()
		{
			WVN_ASSERT(ps_singleton, "ms_singleton must not be nullptr");
			return *ps_singleton;
		}

		static T* get_singleton_ptr()
		{
			return ps_singleton;
		}

	protected:
		static T* ps_singleton;
	};
}

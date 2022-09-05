#pragma once

#include <wvn/util/assert.h>

#define WVN_DEF_SINGLETON(_class) \
public: \
	static _class& get_singleton(); \
	static _class* get_singleton_ptr(); \
private:

#define WVN_IMPL_SINGLETON(_class) \
	template <> _class* Singleton<_class>::ms_singleton = nullptr; \
	_class& _class::get_singleton() { WVN_ASSERT(ms_singleton, "ms_singleton must not be nullptr"); return *ms_singleton; } \
	_class* _class::get_singleton_ptr() { return ms_singleton; }

namespace wvn
{
	template <class T>
	class Singleton
	{
	public:
		Singleton()
		{
			WVN_ASSERT(!ms_singleton, "ms_singleton must be nullptr");
			ms_singleton = static_cast<T*>(this);
		}

		~Singleton()
		{
			WVN_ASSERT(ms_singleton, "ms_singleton must not be nullptr");
			ms_singleton = nullptr;
		}

		static T& get_singleton()
		{
			WVN_ASSERT(ms_singleton, "ms_singleton must not be nullptr");
			return *ms_singleton;
		}

		static T* get_singleton_ptr()
		{
			return ms_singleton;
		}

	protected:
		static T* ms_singleton;
	};
}

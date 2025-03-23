#ifndef PROFILER_H_
#define PROFILER_H_

#include <wvn/common.h>
#include <wvn/root.h>
#include <wvn/singleton.h>

namespace wvn::dev
{
	/**
	 * Profile Manager.
	 */
	class ProfileMgr : public Singleton<ProfileMgr>
	{
		wvn_DEF_SINGLETON(ProfileMgr);

	public:
		ProfileMgr();
		~ProfileMgr();

		void store_sample(const char* name, u64 elapsed)
		{
			printf("PROFILER: %s | %llu\n", name, elapsed);
		}
	};

	/**
	 * Automatic Profiler.
	 */
	class AutoProfiler
	{
	public:
		AutoProfiler(const char* name)
			: m_name(name)
			, m_start_t(Root::get_singleton()->ticks())
		{
		}

		~AutoProfiler()
		{
			u64 end_t = Root::get_singleton()->ticks();
			u64 elapsed = end_t - m_start_t;
			ProfileMgr::get_singleton()->store_sample(m_name, elapsed);
		}

	private:
		const char* m_name;
		u64 m_start_t;
	};
}

#define wvn_PROFILE(name) ::wvn::dev::AutoProfiler __wvn_profiler_scope_##name(#name);

#endif // PROFILER_H_

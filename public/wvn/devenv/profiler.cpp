#include <wvn/devenv/profiler.h>

using namespace wvn;
using namespace wvn::dev;

WVN_IMPL_SINGLETON(ProfileMgr);

ProfileMgr::ProfileMgr()
{
}

ProfileMgr::~ProfileMgr()
{
}

void ProfileMgr::store_sample(const char* name, u64 elapsed)
{
}

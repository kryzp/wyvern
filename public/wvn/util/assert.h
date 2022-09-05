#pragma once

#ifdef WVN_DEBUG

#define WVN_ASSERT(_exp, _msg) if(!(_exp)){/*::wvn::log::error("ASSERTION: " _msg);*/*((volatile int*)0)=0;}
#define WVN_ERROR(_msg) {/*::wvn::log::error("ERROR: " _msg);*/*((volatile int*)0)=0;}

#else

#define WVN_ASSERT(_exp, _msg)
#define WVN_ERROR(_msg)

#endif

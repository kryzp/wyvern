#ifndef ASSERT_H
#define ASSERT_H

#ifdef WVN_DEBUG

#include <stdio.h>

#define WVN_ASSERT(_exp, _msg) do{if(!(_exp)){::printf(_msg "\n");*((volatile int*)0)=0;}}while(0)
#define WVN_ERROR(_msg) ::printf(_msg "\n");*((volatile int*)0)=0

#else

#define WVN_ASSERT(_exp, _msg)
#define WVN_ERROR(_msg)

#endif

#endif // ASSERT_H

#pragma once

#ifdef WVN_DEBUG

#include <stdio.h>

#define WVN_ASSERT(_exp, _msg) if(!(_exp)){printf(_msg "\n");*((volatile int*)0)=0;}
#define WVN_ERROR(_msg) {printf(_msg "\n");*((volatile int*)0)=0;}

#else

#define WVN_ASSERT(_exp, _msg)
#define WVN_ERROR(_msg)

#endif

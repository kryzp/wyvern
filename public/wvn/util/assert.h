#pragma once

#ifdef WVN_DEBUG

#include <stdio.h>

// note: do{}while loop is around just to make sure it always has a ; after it cuz ocd :/
#define WVN_ASSERT(_exp, _msg) do{if(!(_exp)){printf(_msg "\n");*((volatile int*)0)=0;}}while(0)
#define WVN_ERROR(_msg) printf(_msg "\n");*((volatile int*)0)=0

#else

#define WVN_ASSERT(_exp, _msg)
#define WVN_ERROR(_msg)

#endif

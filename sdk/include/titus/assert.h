#pragma once

#include <stdio.h>

#ifndef NDEBUG
#if __GNUC__
#define TITUS_ASSERT(c)                                                                                                \
    if(!(c)) {                                                                                                         \
        printf("ASSERT: %s:%d [%s]\n", __FILE__, __LINE__, __func__);                                                  \
        __builtin_trap();                                                                                              \
    }
#elif _MSC_VER
#define TITUS_ASSERT(c)                                                                                                \
    if(!(c)) {                                                                                                         \
        printf("ASSERT: %s:%d [%s]\n", __FILE__, __LINE__, __func__);                                                  \
        __debugbreak();                                                                                                \
    }
#else
#define TITUS_ASSERT(c)                                                                                                \
    if(!(c)) {                                                                                                         \
        printf("ASSERT: %s:%d [%s]\n", __FILE__, __LINE__, __func__);                                                  \
        *(volatile int*)0 = 0;                                                                                         \
    }
#endif
#else
#define assert(c)
#endif

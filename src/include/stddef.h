/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#include <__cake_types.h>

#define __STDC_VERSION_STDDEF_H__ 202311L

typedef __cake_ptrdiff_t ptrdiff_t;
typedef __cake_size_t size_t;
typedef __cake_wchar_t wchar_t;

typedef struct {
  long long __max_align_ll;
  long double __max_align_ld;
} max_align_t;

typedef typeof(nullptr) nullptr_t;

#ifndef NULL
#define NULL ((void*)0)
#endif

#define offsetof(type, member) __builtin_offsetof(type, member)

#if defined(__GNUC__)
[[noreturn]] void __builtin_unreachable(void);
#define unreachable() __builtin_unreachable()
#elif defined(_WIN32)
/* cl intrinsic; declared so that cake sees the call as not returning */
[[noreturn]] void __assume(int);
#define unreachable() __assume(0)
#else
#define unreachable() do {} while(0)
#endif

#else
#include_next <stddef.h>
#endif

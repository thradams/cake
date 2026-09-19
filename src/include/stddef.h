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

#define unreachable() do {} while(0)

#else
#include_next <stddef.h>
#endif

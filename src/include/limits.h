/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#include <__cake_types.h>

#define __STDC_VERSION_LIMITS_H__ 202311L

#define BOOL_WIDTH   1
#define BOOL_MAX     1

#define CHAR_BIT     8
#if defined(_WIN32)
#define MB_LEN_MAX   5
#elif defined(__APPLE__)
#define MB_LEN_MAX   6
#else
#define MB_LEN_MAX   66
#endif

#define SCHAR_MIN    (-0x7f - 1)
#define SCHAR_MAX    0x7f
#define SCHAR_WIDTH  8
#define UCHAR_MAX    0xff
#define UCHAR_WIDTH  8

#if defined(__CHAR_UNSIGNED__) || defined(___CATALINA__) || defined(__UNSIGNEDCHAR__)
#define CHAR_MIN     0
#define CHAR_MAX     UCHAR_MAX
#else
#define CHAR_MIN     SCHAR_MIN
#define CHAR_MAX     SCHAR_MAX
#endif
#define CHAR_WIDTH   8

#define SHRT_MIN     (-0x7fff - 1)
#define SHRT_MAX     0x7fff
#define SHRT_WIDTH   16
#define USHRT_MAX    0xffff
#define USHRT_WIDTH  16

#if __cake_sizeof_int == 2
#define INT_MIN      (-0x7fff - 1)
#define INT_MAX      0x7fff
#define INT_WIDTH    16
#define UINT_MAX     0xffffU
#define UINT_WIDTH   16
#else
#define INT_MIN      (-0x7fffffff - 1)
#define INT_MAX      0x7fffffff
#define INT_WIDTH    32
#define UINT_MAX     0xffffffffU
#define UINT_WIDTH   32
#endif

#if __cake_sizeof_long == 8
#define LONG_MIN     (-0x7fffffffffffffffL - 1L)
#define LONG_MAX     0x7fffffffffffffffL
#define LONG_WIDTH   64
#define ULONG_MAX    0xffffffffffffffffUL
#define ULONG_WIDTH  64
#else
#define LONG_MIN     (-0x7fffffffL - 1L)
#define LONG_MAX     0x7fffffffL
#define LONG_WIDTH   32
#define ULONG_MAX    0xffffffffUL
#define ULONG_WIDTH  32
#endif

#define LLONG_MIN    (-0x7fffffffffffffffLL - 1LL)
#define LLONG_MAX    0x7fffffffffffffffLL
#define LLONG_WIDTH  64
#define ULLONG_MAX   0xffffffffffffffffULL
#define ULLONG_WIDTH 64

#define BITINT_MAXWIDTH 64

#else
#include_next <limits.h>
#endif

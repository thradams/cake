/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#include <__cake_types.h>

#define __STDC_VERSION_STDINT_H__ 202311L

/* exact width */
typedef __cake_int8_t   int8_t;
typedef __cake_int16_t  int16_t;
typedef __cake_int32_t  int32_t;
typedef __cake_int64_t  int64_t;
typedef __cake_uint8_t  uint8_t;
typedef __cake_uint16_t uint16_t;
typedef __cake_uint32_t uint32_t;
typedef __cake_uint64_t uint64_t;

/* minimum width */
typedef int8_t   int_least8_t;
typedef int16_t  int_least16_t;
typedef int32_t  int_least32_t;
typedef int64_t  int_least64_t;
typedef uint8_t  uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;

/* fastest minimum width */
typedef int8_t   int_fast8_t;
typedef int32_t  int_fast16_t;
typedef int32_t  int_fast32_t;
typedef int64_t  int_fast64_t;
typedef uint8_t  uint_fast8_t;
typedef uint32_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
typedef uint64_t uint_fast64_t;

/* pointer */
typedef __cake_intptr_t  intptr_t;
typedef __cake_uintptr_t uintptr_t;

/* greatest width */
typedef int64_t  intmax_t;
typedef uint64_t uintmax_t;

#define INT8_MIN    (-127 - 1)
#define INT16_MIN   (-32767 - 1)
#define INT32_MIN   (-2147483647 - 1)
#define INT64_MIN   (-__cake_INT64_C(9223372036854775807) - 1)
#define INT8_MAX    127
#define INT16_MAX   32767
#define INT32_MAX   2147483647
#define INT64_MAX   __cake_INT64_C(9223372036854775807)
#define UINT8_MAX   255
#define UINT16_MAX  65535
#define UINT32_MAX  4294967295U
#define UINT64_MAX  __cake_UINT64_C(18446744073709551615)

#define INT8_WIDTH    8
#define INT16_WIDTH   16
#define INT32_WIDTH   32
#define INT64_WIDTH   64
#define UINT8_WIDTH   8
#define UINT16_WIDTH  16
#define UINT32_WIDTH  32
#define UINT64_WIDTH  64

#define INT_LEAST8_MIN    INT8_MIN
#define INT_LEAST16_MIN   INT16_MIN
#define INT_LEAST32_MIN   INT32_MIN
#define INT_LEAST64_MIN   INT64_MIN
#define INT_LEAST8_MAX    INT8_MAX
#define INT_LEAST16_MAX   INT16_MAX
#define INT_LEAST32_MAX   INT32_MAX
#define INT_LEAST64_MAX   INT64_MAX
#define UINT_LEAST8_MAX   UINT8_MAX
#define UINT_LEAST16_MAX  UINT16_MAX
#define UINT_LEAST32_MAX  UINT32_MAX
#define UINT_LEAST64_MAX  UINT64_MAX

#define INT_LEAST8_WIDTH   8
#define INT_LEAST16_WIDTH  16
#define INT_LEAST32_WIDTH  32
#define INT_LEAST64_WIDTH  64
#define UINT_LEAST8_WIDTH  8
#define UINT_LEAST16_WIDTH 16
#define UINT_LEAST32_WIDTH 32
#define UINT_LEAST64_WIDTH 64

#define INT_FAST8_MIN     INT8_MIN
#define INT_FAST16_MIN    INT32_MIN
#define INT_FAST32_MIN    INT32_MIN
#define INT_FAST64_MIN    INT64_MIN
#define INT_FAST8_MAX     INT8_MAX
#define INT_FAST16_MAX    INT32_MAX
#define INT_FAST32_MAX    INT32_MAX
#define INT_FAST64_MAX    INT64_MAX
#define UINT_FAST8_MAX    UINT8_MAX
#define UINT_FAST16_MAX   UINT32_MAX
#define UINT_FAST32_MAX   UINT32_MAX
#define UINT_FAST64_MAX   UINT64_MAX

#define INT_FAST8_WIDTH   8
#define INT_FAST16_WIDTH  32
#define INT_FAST32_WIDTH  32
#define INT_FAST64_WIDTH  64
#define UINT_FAST8_WIDTH  8
#define UINT_FAST16_WIDTH 32
#define UINT_FAST32_WIDTH 32
#define UINT_FAST64_WIDTH 64

#if __cake_sizeof_pointer == 8
/* the suffix must produce the type of intptr_t/ptrdiff_t/size_t, which is
   long on LP64 (linux, macOS) and long long on windows */
#if __cake_sizeof_long == 8
#define INTPTR_MIN     (-9223372036854775807L - 1)
#define INTPTR_MAX     9223372036854775807L
#define UINTPTR_MAX    18446744073709551615UL
#else
#define INTPTR_MIN     (-9223372036854775807LL - 1)
#define INTPTR_MAX     9223372036854775807LL
#define UINTPTR_MAX    18446744073709551615ULL
#endif
#define INTPTR_WIDTH   64
#define UINTPTR_WIDTH  64
#define PTRDIFF_MIN    INTPTR_MIN
#define PTRDIFF_MAX    INTPTR_MAX
#define PTRDIFF_WIDTH  64
#define SIZE_MAX       UINTPTR_MAX
#define SIZE_WIDTH     64
#else
#define INTPTR_MIN     INT32_MIN
#define INTPTR_MAX     INT32_MAX
#define UINTPTR_MAX    UINT32_MAX
#define INTPTR_WIDTH   32
#define UINTPTR_WIDTH  32
#if __cake_sizeof_int == 2
#define PTRDIFF_MIN    INT16_MIN
#define PTRDIFF_MAX    INT16_MAX
#define PTRDIFF_WIDTH  16
#define SIZE_MAX       UINT16_MAX
#define SIZE_WIDTH     16
#else
#define PTRDIFF_MIN    INT32_MIN
#define PTRDIFF_MAX    INT32_MAX
#define PTRDIFF_WIDTH  32
#define SIZE_MAX       UINT32_MAX
#define SIZE_WIDTH     32
#endif
#endif

#define INTMAX_MIN     INT64_MIN
#define INTMAX_MAX     INT64_MAX
#define UINTMAX_MAX    UINT64_MAX
#define INTMAX_WIDTH   64
#define UINTMAX_WIDTH  64

#define SIG_ATOMIC_MIN   INT32_MIN
#define SIG_ATOMIC_MAX   INT32_MAX
#define SIG_ATOMIC_WIDTH 32

#if defined(_WIN32) || !defined(__SIZE_TYPE__)
/* wchar_t is unsigned short */
#define WCHAR_MIN   0
#define WCHAR_MAX   0xffff
#define WCHAR_WIDTH 16
#define WINT_MIN    0
#define WINT_MAX    0xffff
#define WINT_WIDTH  16
#elif defined(__APPLE__)
/* wchar_t and wint_t are int */
#define WCHAR_MIN   INT32_MIN
#define WCHAR_MAX   INT32_MAX
#define WCHAR_WIDTH 32
#define WINT_MIN    INT32_MIN
#define WINT_MAX    INT32_MAX
#define WINT_WIDTH  32
#else
/* wchar_t is int, wint_t is unsigned int */
#define WCHAR_MIN   INT32_MIN
#define WCHAR_MAX   INT32_MAX
#define WCHAR_WIDTH 32
#define WINT_MIN    0U
#define WINT_MAX    UINT32_MAX
#define WINT_WIDTH  32
#endif

#define INT8_C(c)    c
#define INT16_C(c)   c
#define INT32_C(c)   c
#define INT64_C(c)   __cake_INT64_C(c)
#define UINT8_C(c)   c
#define UINT16_C(c)  c
#define UINT32_C(c)  c ## U
#define UINT64_C(c)  __cake_UINT64_C(c)
#define INTMAX_C(c)  __cake_INT64_C(c)
#define UINTMAX_C(c) __cake_UINT64_C(c)

#else
#include_next <stdint.h>
#endif

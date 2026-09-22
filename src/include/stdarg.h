/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#include <__cake_types.h>

#define __STDC_VERSION_STDARG_H__ 202311L

typedef __cake_va_list va_list;

#if defined(_WIN32)

/* same implementation as the msvc headers: the generated code is compiled
   by cl, which does not have the gcc builtins */
#define __cake_ADDRESSOF(v) (&(v))

#if defined(_M_X64)

void __cdecl __va_start(va_list*, ...);

/* C23 allows va_start(ap) with no second argument; __va_start accepts it */
#define va_start(ap, ...) ((void)(__va_start(&ap __VA_OPT__(,) __VA_ARGS__)))
#define va_arg(ap, t)                                                    \
    ((sizeof(t) > sizeof(__int64) || (sizeof(t) & (sizeof(t) - 1)) != 0) \
        ? **(t**)((ap += sizeof(__int64)) - sizeof(__int64))             \
        :  *(t* )((ap += sizeof(__int64)) - sizeof(__int64)))
#define va_end(ap) ((void)(ap = (va_list)0))

#else /* x86 */

/* x86 needs the address of the last named parameter, so the C23 single
   argument form va_start(ap) is not available on this target */

#define __cake_INTSIZEOF(n) ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))

#define va_start(ap, v) ((void)(ap = (va_list)__cake_ADDRESSOF(v) + __cake_INTSIZEOF(v)))
#define va_arg(ap, t)   (*(t*)((ap += __cake_INTSIZEOF(t)) - __cake_INTSIZEOF(t)))
#define va_end(ap)      ((void)(ap = (va_list)0))

#endif

#define va_copy(destination, source) ((destination) = (source))

#else

/* gcc/clang and the small targets */
#define va_start(...)     __builtin_va_start(__VA_ARGS__)
#define va_arg(ap, type)  __builtin_va_arg(ap, type)
#define va_end(ap)        __builtin_va_end(ap)
#define va_copy(dst, src) __builtin_va_copy(dst, src)

#endif

#else
#include_next <stdarg.h>
#endif

/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#define __STDC_VERSION_STDCKDINT_H__ 202311L

#if defined(__GNUC__)

/* cake does not check the arguments of __builtin_* functions declared with
   an empty parameter list, and gcc/clang compile the generated calls */
bool __builtin_add_overflow();
bool __builtin_sub_overflow();
bool __builtin_mul_overflow();

#define ckd_add(result, a, b) __builtin_add_overflow((a), (b), (result))
#define ckd_sub(result, a, b) __builtin_sub_overflow((a), (b), (result))
#define ckd_mul(result, a, b) __builtin_mul_overflow((a), (b), (result))

#else
#error <stdckdint.h> with -cake-headers needs the gcc/clang overflow builtins
#endif

#else
#include_next <stdckdint.h>
#endif

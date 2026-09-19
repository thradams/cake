/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

/*
  jmp_buf is filled by the libc, so it must be at least as big (and as
  aligned) as the real one:
    glibc x86_64      200 bytes, align 8
    macOS arm64       192 bytes, align 4
    msvc x64          256 bytes, align 16
    msvc x86          64 bytes, align 4
*/
#if defined(_WIN64)
typedef struct { _Alignas(16) long long __cake_buf[32]; } jmp_buf[1];
#else
typedef struct { long long __cake_buf[32]; } jmp_buf[1];
#endif

#if defined(_WIN32)
/* cl treats _setjmp as an intrinsic and supplies the frame context itself */
int _setjmp(jmp_buf env);
#define setjmp _setjmp
#else
int setjmp(jmp_buf env);
#endif
[[noreturn]] void longjmp(jmp_buf env, int val);

#else
#include_next <setjmp.h>
#endif

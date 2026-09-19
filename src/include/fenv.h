/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#define __STDC_VERSION_FENV_H__ 202311L

/* the constants and the environment layout are those of the target libc */
#if defined(_WIN32)

#define FE_INVALID   0x01
#define FE_DENORMAL  0x02
#define FE_DIVBYZERO 0x04
#define FE_OVERFLOW  0x08
#define FE_UNDERFLOW 0x10
#define FE_INEXACT   0x20
#define FE_ALL_EXCEPT (FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

#define FE_TONEAREST  0x0000
#define FE_UPWARD     0x0100
#define FE_DOWNWARD   0x0200
#define FE_TOWARDZERO 0x0300

typedef unsigned long fexcept_t;
typedef struct { unsigned long _Fe_ctl; unsigned long _Fe_stat; } fenv_t;

/* the CRT header defines the default environment itself (selectany) */
#if defined(_M_IX86)
static const fenv_t __cake_fe_dfl_env = { 0x3f3f103f, 0 };
#else
static const fenv_t __cake_fe_dfl_env = { 0x3f00003f, 0 };
#endif
#define FE_DFL_ENV (&__cake_fe_dfl_env)

#elif defined(__APPLE__)

#define FE_INVALID   0x01
#define FE_DIVBYZERO 0x02
#define FE_OVERFLOW  0x04
#define FE_UNDERFLOW 0x08
#define FE_INEXACT   0x10
#define FE_ALL_EXCEPT (FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

#define FE_TONEAREST  0x00000000
#define FE_UPWARD     0x00400000
#define FE_DOWNWARD   0x00800000
#define FE_TOWARDZERO 0x00c00000

typedef unsigned long long fexcept_t;
typedef struct { unsigned long long __fpsr; unsigned long long __fpcr; } fenv_t;

extern const fenv_t _FE_DFL_ENV;
#define FE_DFL_ENV (&_FE_DFL_ENV)

#else /* glibc x86_64 */

#define FE_INVALID   0x01
#define FE_DIVBYZERO 0x04
#define FE_OVERFLOW  0x08
#define FE_UNDERFLOW 0x10
#define FE_INEXACT   0x20
#define FE_ALL_EXCEPT (FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

#define FE_TONEAREST  0
#define FE_DOWNWARD   0x400
#define FE_UPWARD     0x800
#define FE_TOWARDZERO 0xc00

typedef unsigned short int fexcept_t;

typedef struct
{
    unsigned short int __control_word;
    unsigned short int __glibc_reserved1;
    unsigned short int __status_word;
    unsigned short int __glibc_reserved2;
    unsigned short int __tags;
    unsigned short int __glibc_reserved3;
    unsigned int __eip;
    unsigned short int __cs_selector;
    unsigned int __opcode : 11;
    unsigned int __glibc_reserved4 : 5;
    unsigned int __data_offset;
    unsigned short int __data_selector;
    unsigned short int __glibc_reserved5;
    unsigned int __mxcsr;
} fenv_t;

#define FE_DFL_ENV ((const fenv_t*) -1)

typedef struct
{
    unsigned short int __control_word;
    unsigned short int __glibc_reserved;
    unsigned int __mxcsr;
} femode_t;

#define FE_DFL_MODE ((const femode_t*) -1L)

/* C23 additions, glibc only for now */
int fesetexcept(int excepts);
int fetestexceptflag(const fexcept_t* flagp, int excepts);
int fegetmode(femode_t* modep);
int fesetmode(const femode_t* modep);

#endif

/* exceptions */
int feclearexcept(int excepts);
int fegetexceptflag(fexcept_t* flagp, int excepts);
int fesetexceptflag(const fexcept_t* flagp, int excepts);
int fetestexcept(int excepts);

/* rounding */
int fegetround(void);
int fesetround(int rnd);

/* environment */
int fegetenv(fenv_t* envp);
int feholdexcept(fenv_t* envp);
int fesetenv(const fenv_t* envp);

#if defined(_WIN32)

/* the msvc CRT does not export these two: feraiseexcept is inline so that it
   is compiled with the /arch of the user program, feupdateenv calls it */
static inline int feraiseexcept(int excepts)
{
    static const struct { int except; double num; double denom; } table[] = {
        { FE_INVALID,   0.0,    0.0    },
        { FE_DIVBYZERO, 1.0,    0.0    },
        { FE_OVERFLOW,  1e+300, 1e-300 },
        { FE_UNDERFLOW, 1e-300, 1e+300 },
        { FE_INEXACT,   2.0,    3.0    }
    };
    volatile double ans = 0.0;
    (void)ans;

    if ((excepts &= FE_ALL_EXCEPT) == 0)
        return 0;

    for (unsigned i = 0; i < sizeof(table) / sizeof(table[0]); i++)
    {
        if ((excepts & table[i].except) != 0)
            ans = table[i].num / table[i].denom;
    }
    return 0;
}

static inline int feupdateenv(const fenv_t* envp)
{
    int excepts = fetestexcept(FE_ALL_EXCEPT);
    if (fesetenv(envp) != 0 || feraiseexcept(excepts) != 0)
        return 1;
    return 0;
}

#else
int feraiseexcept(int excepts);
int feupdateenv(const fenv_t* envp);
#endif

#else
#include_next <fenv.h>
#endif

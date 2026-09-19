/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#define __STDC_VERSION_MATH_H__ 202311L

typedef float float_t;
typedef double double_t;

#if defined(__GNUC__)
#define HUGE_VAL  (__builtin_huge_val())
#define HUGE_VALF (__builtin_huge_valf())
#define HUGE_VALL (__builtin_huge_vall())
#define INFINITY  (__builtin_inff())
#define NAN       (__builtin_nanf(""))
#else
#define HUGE_VAL  ((double)(1e300 * 1e300))
#define HUGE_VALF ((float)HUGE_VAL)
#define HUGE_VALL ((long double)HUGE_VAL)
#define INFINITY  HUGE_VALF
#define NAN       ((float)(INFINITY * 0.0F))
#endif

/*
  Classification. The macros expand to the real libm functions of each
  target, and the FP_* values are the ones of that libm.
*/
#if defined(_WIN32)

#define FP_INFINITE  1
#define FP_NAN       2
#define FP_NORMAL    (-1)
#define FP_SUBNORMAL (-2)
#define FP_ZERO      0

short _dclass(double x);
short _fdclass(float x);
short _ldclass(long double x);
int _dsign(double x);
int _fdsign(float x);
int _ldsign(long double x);

#define fpclassify(x) _Generic((x), float: _fdclass, long double: _ldclass, default: _dclass)(x)
#define signbit(x)    _Generic((x), float: _fdsign, long double: _ldsign, default: _dsign)(x)

#elif defined(__APPLE__)

#define FP_NAN       1
#define FP_INFINITE  2
#define FP_ZERO      3
#define FP_NORMAL    4
#define FP_SUBNORMAL 5

int __fpclassifyd(double x);
int __fpclassifyf(float x);
int __fpclassifyl(long double x);
int __signbitd(double x);
int __signbitf(float x);
int __signbitl(long double x);

#define fpclassify(x) _Generic((x), float: __fpclassifyf, long double: __fpclassifyl, default: __fpclassifyd)(x)
#define signbit(x)    _Generic((x), float: __signbitf, long double: __signbitl, default: __signbitd)(x)

#else /* glibc */

#define FP_NAN       0
#define FP_INFINITE  1
#define FP_ZERO      2
#define FP_SUBNORMAL 3
#define FP_NORMAL    4

int __fpclassify(double x);
int __fpclassifyf(float x);
int __fpclassifyl(long double x);
int __signbit(double x);
int __signbitf(float x);
int __signbitl(long double x);

#define fpclassify(x) _Generic((x), float: __fpclassifyf, long double: __fpclassifyl, default: __fpclassify)(x)
#define signbit(x)    _Generic((x), float: __signbitf, long double: __signbitl, default: __signbit)(x)

#endif

#define FP_ILOGB0   (-2147483647 - 1)
#define FP_ILOGBNAN (-2147483647 - 1)

#define MATH_ERRNO     1
#define MATH_ERREXCEPT 2
#if defined(__APPLE__)
#define math_errhandling MATH_ERREXCEPT
#else
#define math_errhandling (MATH_ERRNO | MATH_ERREXCEPT)
#endif

#define isfinite(x)   (fpclassify(x) != FP_INFINITE && fpclassify(x) != FP_NAN)
#define isinf(x)      (fpclassify(x) == FP_INFINITE)
#define isnan(x)      (fpclassify(x) == FP_NAN)
#define isnormal(x)   (fpclassify(x) == FP_NORMAL)
#define issubnormal(x) (fpclassify(x) == FP_SUBNORMAL)
#define iszero(x)     (fpclassify(x) == FP_ZERO)

#define isgreater(x, y)      ((x) > (y))
#define isgreaterequal(x, y) ((x) >= (y))
#define isless(x, y)         ((x) < (y))
#define islessequal(x, y)    ((x) <= (y))
#define islessgreater(x, y)  ((x) < (y) || (x) > (y))
#define isunordered(x, y)    (isnan(x) || isnan(y))

/* double */
double acos(double x);
double asin(double x);
double atan(double x);
double atan2(double y, double x);
double cos(double x);
double sin(double x);
double tan(double x);
double acosh(double x);
double asinh(double x);
double atanh(double x);
double cosh(double x);
double sinh(double x);
double tanh(double x);
double exp(double x);
double exp2(double x);
double expm1(double x);
double frexp(double value, int* exp);
int ilogb(double x);
double ldexp(double x, int exp);
double log(double x);
double log10(double x);
double log1p(double x);
double log2(double x);
double logb(double x);
double modf(double value, double* iptr);
double scalbn(double x, int n);
double scalbln(double x, long int n);
double cbrt(double x);
double fabs(double x);
double hypot(double x, double y);
double pow(double x, double y);
double sqrt(double x);
double erf(double x);
double erfc(double x);
double lgamma(double x);
double tgamma(double x);
double ceil(double x);
double floor(double x);
double nearbyint(double x);
double rint(double x);
long int lrint(double x);
long long int llrint(double x);
double round(double x);
long int lround(double x);
long long int llround(double x);
double trunc(double x);
double fmod(double x, double y);
double remainder(double x, double y);
double remquo(double x, double y, int* quo);
double copysign(double x, double y);
double nan(const char* tagp);
double nextafter(double x, double y);
double nexttoward(double x, long double y);
double fdim(double x, double y);
double fmax(double x, double y);
double fmin(double x, double y);
double fma(double x, double y, double z);

/* the msvc CRT does not export every float/long double variant: the ones
   below are inline wrappers over the double function in its headers
   (x86 has fewer float symbols than x64) */
/* float */
#if defined(_WIN32) && !defined(_WIN64)
static inline float acosf(float x) { return (float)acos((double)x); }
#else
float acosf(float x);
#endif
#if defined(_WIN32) && !defined(_WIN64)
static inline float asinf(float x) { return (float)asin((double)x); }
#else
float asinf(float x);
#endif
#if defined(_WIN32) && !defined(_WIN64)
static inline float atanf(float x) { return (float)atan((double)x); }
#else
float atanf(float x);
#endif
float atan2f(float y, float x);
float cosf(float x);
float sinf(float x);
float tanf(float x);
float acoshf(float x);
float asinhf(float x);
float atanhf(float x);
#if defined(_WIN32) && !defined(_WIN64)
static inline float coshf(float x) { return (float)cosh((double)x); }
#else
float coshf(float x);
#endif
float sinhf(float x);
#if defined(_WIN32) && !defined(_WIN64)
static inline float tanhf(float x) { return (float)tanh((double)x); }
#else
float tanhf(float x);
#endif
float expf(float x);
float exp2f(float x);
float expm1f(float x);
#if defined(_WIN32)
static inline float frexpf(float value, int* exp) { return (float)frexp((double)value, exp); }
#else
float frexpf(float value, int* exp);
#endif
int ilogbf(float x);
float ldexpf(float x, int exp);
float logf(float x);
#if defined(_WIN32) && !defined(_WIN64)
static inline float log10f(float x) { return (float)log10((double)x); }
#else
float log10f(float x);
#endif
float log1pf(float x);
float log2f(float x);
float logbf(float x);
#if defined(_WIN32) && !defined(_WIN64)
static inline float modff(float value, float* iptr) { double i; double f = modf((double)value, &i); *iptr = (float)i; return (float)f; }
#else
float modff(float value, float* iptr);
#endif
float scalbnf(float x, int n);
float scalblnf(float x, long int n);
float cbrtf(float x);
#if defined(_WIN32)
static inline float fabsf(float x) { return (float)fabs((double)x); }
#else
float fabsf(float x);
#endif
float hypotf(float x, float y);
float powf(float x, float y);
float sqrtf(float x);
float erff(float x);
float erfcf(float x);
float lgammaf(float x);
float tgammaf(float x);
#if defined(_WIN32) && !defined(_WIN64)
static inline float ceilf(float x) { return (float)ceil((double)x); }
#else
float ceilf(float x);
#endif
#if defined(_WIN32) && !defined(_WIN64)
static inline float floorf(float x) { return (float)floor((double)x); }
#else
float floorf(float x);
#endif
float nearbyintf(float x);
float rintf(float x);
long int lrintf(float x);
long long int llrintf(float x);
float roundf(float x);
long int lroundf(float x);
long long int llroundf(float x);
float truncf(float x);
float fmodf(float x, float y);
float remainderf(float x, float y);
float remquof(float x, float y, int* quo);
float copysignf(float x, float y);
float nanf(const char* tagp);
float nextafterf(float x, float y);
float nexttowardf(float x, long double y);
float fdimf(float x, float y);
float fmaxf(float x, float y);
float fminf(float x, float y);
float fmaf(float x, float y, float z);

/* long double */
#if defined(_WIN32)
static inline long double acosl(long double x) { return (long double)acos((double)x); }
#else
long double acosl(long double x);
#endif
#if defined(_WIN32)
static inline long double asinl(long double x) { return (long double)asin((double)x); }
#else
long double asinl(long double x);
#endif
#if defined(_WIN32)
static inline long double atanl(long double x) { return (long double)atan((double)x); }
#else
long double atanl(long double x);
#endif
long double atan2l(long double y, long double x);
long double cosl(long double x);
long double sinl(long double x);
long double tanl(long double x);
long double acoshl(long double x);
long double asinhl(long double x);
long double atanhl(long double x);
#if defined(_WIN32)
static inline long double coshl(long double x) { return (long double)cosh((double)x); }
#else
long double coshl(long double x);
#endif
long double sinhl(long double x);
#if defined(_WIN32)
static inline long double tanhl(long double x) { return (long double)tanh((double)x); }
#else
long double tanhl(long double x);
#endif
long double expl(long double x);
long double exp2l(long double x);
long double expm1l(long double x);
#if defined(_WIN32)
static inline long double frexpl(long double value, int* exp) { return (long double)frexp((double)value, exp); }
#else
long double frexpl(long double value, int* exp);
#endif
int ilogbl(long double x);
long double ldexpl(long double x, int exp);
long double logl(long double x);
#if defined(_WIN32)
static inline long double log10l(long double x) { return (long double)log10((double)x); }
#else
long double log10l(long double x);
#endif
long double log1pl(long double x);
long double log2l(long double x);
long double logbl(long double x);
#if defined(_WIN32)
static inline long double modfl(long double value, long double* iptr) { double i; double f = modf((double)value, &i); *iptr = (long double)i; return (long double)f; }
#else
long double modfl(long double value, long double* iptr);
#endif
long double scalbnl(long double x, int n);
long double scalblnl(long double x, long int n);
long double cbrtl(long double x);
#if defined(_WIN32)
static inline long double fabsl(long double x) { return (long double)fabs((double)x); }
#else
long double fabsl(long double x);
#endif
long double hypotl(long double x, long double y);
long double powl(long double x, long double y);
long double sqrtl(long double x);
long double erfl(long double x);
long double erfcl(long double x);
long double lgammal(long double x);
long double tgammal(long double x);
#if defined(_WIN32)
static inline long double ceill(long double x) { return (long double)ceil((double)x); }
#else
long double ceill(long double x);
#endif
#if defined(_WIN32)
static inline long double floorl(long double x) { return (long double)floor((double)x); }
#else
long double floorl(long double x);
#endif
long double nearbyintl(long double x);
long double rintl(long double x);
long int lrintl(long double x);
long long int llrintl(long double x);
long double roundl(long double x);
long int lroundl(long double x);
long long int llroundl(long double x);
long double truncl(long double x);
long double fmodl(long double x, long double y);
long double remainderl(long double x, long double y);
long double remquol(long double x, long double y, int* quo);
long double copysignl(long double x, long double y);
long double nanl(const char* tagp);
long double nextafterl(long double x, long double y);
long double nexttowardl(long double x, long double y);
long double fdiml(long double x, long double y);
long double fmaxl(long double x, long double y);
long double fminl(long double x, long double y);
long double fmal(long double x, long double y, long double z);

#else
#include_next <math.h>
#endif

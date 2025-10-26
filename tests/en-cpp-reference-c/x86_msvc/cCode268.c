/* Cake 0.12.26 x86_msvc */
struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


int __cdecl fesetround(int _Round);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
double __cdecl rint(double _X);
long __cdecl lrint(double _X);
int __cdecl feclearexcept(int _Flags);
int __cdecl fetestexcept(int _Flags);
int __cdecl puts(char * _Buffer);

int main(void)
{
    fesetround(0);
    printf("rounding to nearest (halfway cases to even):\nrint(+2.3) = %+.1f  ", rint(2.2999999999999998));
    printf("rint(+2.5) = %+.1f  ", rint(2.5));
    printf("rint(+3.5) = %+.1f\n", rint(3.5));
    printf("rint(-2.3) = %+.1f  ", rint(-2.2999999999999998));
    printf("rint(-2.5) = %+.1f  ", rint(-2.5));
    printf("rint(-3.5) = %+.1f\n", rint(-3.5));
    fesetround(256);
    printf("rounding down: \nrint(+2.3) = %+.1f  ", rint(2.2999999999999998));
    printf("rint(+2.5) = %+.1f  ", rint(2.5));
    printf("rint(+3.5) = %+.1f\n", rint(3.5));
    printf("rint(-2.3) = %+.1f  ", rint(-2.2999999999999998));
    printf("rint(-2.5) = %+.1f  ", rint(-2.5));
    printf("rint(-3.5) = %+.1f\n", rint(-3.5));
    printf("rounding down with lrint: \nlrint(+2.3) = %ld  ", lrint(2.2999999999999998));
    printf("lrint(+2.5) = %ld  ", lrint(2.5));
    printf("lrint(+3.5) = %ld\n", lrint(3.5));
    printf("lrint(-2.3) = %ld  ", lrint(-2.2999999999999998));
    printf("lrint(-2.5) = %ld  ", lrint(-2.5));
    printf("lrint(-3.5) = %ld\n", lrint(-3.5));
    printf("lrint(-0.0) = %ld\n", lrint(-0.0));
    printf("lrint(-Inf) = %ld\n", lrint(-inff));
    feclearexcept(31);
    printf("rint(1.1) = %.1f\n", rint(1.1000000000000001));
    if (fetestexcept(1))
    {
        puts("    FE_INEXACT was raised");
    }
    feclearexcept(31);
    printf("lrint(LONG_MIN-2048.0) = %ld\n", lrint(-2048.0));
    if (fetestexcept(16))
    {
        puts("    FE_INVALID was raised");
    }
}



static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__c0__OptionsStorage;
}

static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

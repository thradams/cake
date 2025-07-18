struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


int __cdecl fesetround(int _Round);


int __cdecl __stdio_common_vfprintf(unsigned __int64 _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

static unsigned __int64 __ck__OptionsStorage1;
inline unsigned __int64 *__cdecl __local_stdio_printf_options(void)
{
    return &__ck__OptionsStorage1;
}
inline int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
inline int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}
double __cdecl rint(double _X);
long __cdecl lrint(double _X);
int __cdecl feclearexcept(int _Flags);
int __cdecl fetestexcept(int _Flags);
int __cdecl puts(char * _Buffer);

int main(void)
{
    fesetround(0);
    printf("rounding to nearest (halfway cases to even):\n""rint(+2.3) = %+.1f  ", rint(2.300000));
    printf("rint(+2.5) = %+.1f  ", rint(2.500000));
    printf("rint(+3.5) = %+.1f\n", rint(3.500000));
    printf("rint(-2.3) = %+.1f  ", rint(-2.300000));
    printf("rint(-2.5) = %+.1f  ", rint(-2.500000));
    printf("rint(-3.5) = %+.1f\n", rint(-3.500000));
    fesetround(256);
    printf("rounding down: \nrint(+2.3) = %+.1f  ", rint(2.300000));
    printf("rint(+2.5) = %+.1f  ", rint(2.500000));
    printf("rint(+3.5) = %+.1f\n", rint(3.500000));
    printf("rint(-2.3) = %+.1f  ", rint(-2.300000));
    printf("rint(-2.5) = %+.1f  ", rint(-2.500000));
    printf("rint(-3.5) = %+.1f\n", rint(-3.500000));
    printf("rounding down with lrint: \nlrint(+2.3) = %ld  ", lrint(2.300000));
    printf("lrint(+2.5) = %ld  ", lrint(2.500000));
    printf("lrint(+3.5) = %ld\n", lrint(3.500000));
    printf("lrint(-2.3) = %ld  ", lrint(-2.300000));
    printf("lrint(-2.5) = %ld  ", lrint(-2.500000));
    printf("lrint(-3.5) = %ld\n", lrint(-3.500000));
    printf("lrint(-0.0) = %ld\n", lrint(-0.000000));
    printf("lrint(-Inf) = %ld\n", lrint(-1.000000f));
    feclearexcept(31);
    printf("rint(1.1) = %.1f\n", rint(1.100000));
    if (fetestexcept(1))
    {
        puts("    FE_INEXACT was raised");
    }
    feclearexcept(31);
    printf("lrint(LONG_MIN-2048.0) = %ld\n", lrint(-2147485696.000000));
    if (fetestexcept(16))
    {
        puts("    FE_INVALID was raised");
    }
}


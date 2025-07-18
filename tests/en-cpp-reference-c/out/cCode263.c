struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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
double __cdecl nearbyint(double _X);

int main(void)
{
    fesetround(0);
    printf("rounding to nearest:\nnearbyint(+2.3) = %+.1f  ", nearbyint(2.300000));
    printf("nearbyint(+2.5) = %+.1f  ", nearbyint(2.500000));
    printf("nearbyint(+3.5) = %+.1f\n", nearbyint(3.500000));
    printf("nearbyint(-2.3) = %+.1f  ", nearbyint(-2.300000));
    printf("nearbyint(-2.5) = %+.1f  ", nearbyint(-2.500000));
    printf("nearbyint(-3.5) = %+.1f\n", nearbyint(-3.500000));
    fesetround(256);
    printf("rounding down: \nnearbyint(+2.3) = %+.1f  ", nearbyint(2.300000));
    printf("nearbyint(+2.5) = %+.1f  ", nearbyint(2.500000));
    printf("nearbyint(+3.5) = %+.1f\n", nearbyint(3.500000));
    printf("nearbyint(-2.3) = %+.1f  ", nearbyint(-2.300000));
    printf("nearbyint(-2.5) = %+.1f  ", nearbyint(-2.500000));
    printf("nearbyint(-3.5) = %+.1f\n", nearbyint(-3.500000));
    printf("nearbyint(-0.0) = %+.1f\n", nearbyint(-0.000000));
    printf("nearbyint(-Inf) = %+.1f\n", nearbyint(-1.000000f));
}


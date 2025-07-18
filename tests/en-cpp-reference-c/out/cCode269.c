struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};




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
double __cdecl round(double _X);
long __cdecl lround(double _X);
int __cdecl feclearexcept(int _Flags);
int __cdecl fetestexcept(int _Flags);
int __cdecl puts(char * _Buffer);

int main(void)
{
    printf("round(+2.3) = %+.1f  ", round(2.300000));
    printf("round(+2.5) = %+.1f  ", round(2.500000));
    printf("round(+2.7) = %+.1f\n", round(2.700000));
    printf("round(-2.3) = %+.1f  ", round(-2.300000));
    printf("round(-2.5) = %+.1f  ", round(-2.500000));
    printf("round(-2.7) = %+.1f\n", round(-2.700000));
    printf("round(-0.0) = %+.1f\n", round(-0.000000));
    printf("round(-Inf) = %+f\n", round(-1.000000f));
    printf("lround(+2.3) = %ld  ", lround(2.300000));
    printf("lround(+2.5) = %ld  ", lround(2.500000));
    printf("lround(+2.7) = %ld\n", lround(2.700000));
    printf("lround(-2.3) = %ld  ", lround(-2.300000));
    printf("lround(-2.5) = %ld  ", lround(-2.500000));
    printf("lround(-2.7) = %ld\n", lround(-2.700000));
    printf("lround(-0.0) = %ld\n", lround(-0.000000));
    printf("lround(-Inf) = %ld\n", lround(-1.000000f));
    feclearexcept(31);
    printf("lround(LONG_MAX+1.5) = %ld\n", lround(2147483648.500000));
    if (fetestexcept(16))
    {
        puts("    FE_INVALID was raised");
    }
}


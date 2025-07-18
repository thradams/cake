struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


double __cdecl erfc(double _X);
double __cdecl sqrt(double _X);

double normalCDF(double x)
{
    return erfc(-x / sqrt(2)) / 2;
}

int __cdecl puts(char * _Buffer);


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

int main(void)
{
    puts("normal cumulative distribution function:");
    {
        double n;
        n = 0;
        for (; n < 1; n += 0.100000)
        printf("normalCDF(%.2f) %5.2f%%\n", n, 100 * normalCDF(n));
    }
    printf("special values:\n""erfc(-Inf) = %f\n""erfc(Inf) = %f\n", erfc(-1.000000f), erfc(1.000000f));
}


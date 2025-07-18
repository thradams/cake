struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


long __cdecl clock(void);


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
    long t;
    double d;
    double vd;

    t = clock();
    d = 0.000000;
    {
        int n;
        n = 0;
        for (; n < 10000; ++n)
        {
            int m;
            m = 0;
            for (; m < 10000; ++m)
            d += d * n * m;
        }
    }
    printf("Modified a non-volatile variable 100m times. ""Time used: %.2f seconds\n", (double)(clock() - t) / 1000L);
    t = clock();
    vd = 0.000000;
    {
        int n;
        n = 0;
        for (; n < 10000; ++n)
        {
            int m;
            m = 0;
            for (; m < 10000; ++m)
            {
                double prod;

                prod = vd * n * m;
                vd += prod;
            }
        }
    }
    printf("Modified a volatile variable 100m times. ""Time used: %.2f seconds\n", (double)(clock() - t) / 1000L);
}


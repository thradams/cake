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
double __cdecl modf(double _X, double * _Y);
double __cdecl frexp(double _X, int * _Y);
double __cdecl logb(double _X);
double __cdecl scalbn(double _X, int _Y);
int __cdecl feclearexcept(int _Flags);
int __cdecl fetestexcept(int _Flags);
int __cdecl puts(char * _Buffer);

int main(void)
{
    double f;
    double f3;
    double f2;
    int i;

    f = 123.450000;
    printf("Given the number %.2f or %a in hex,\n", f, f);
    f2 = modf(f, &f3);
    printf("modf() makes %.0f + %.2f\n", f3, f2);
    f2 = frexp(f, &i);
    printf("frexp() makes %f * 2^%d\n", f2, i);
    i = logb(f);
    printf("logb()/logb() make %f * %d^%d\n", f / scalbn(1.000000, i), 2, i);
    feclearexcept(31);
    printf("logb(0) = %f\n", logb(0));
    if (fetestexcept(8))
    {
        puts("    FE_DIVBYZERO raised");
    }
}


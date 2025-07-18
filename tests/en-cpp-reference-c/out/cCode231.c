struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


double __cdecl fabs(double _X);
double f(double);

double integrate(double f(double), double a, double b, unsigned int steps)
{
    double dx;
    double sum;

    dx = (b - a) / steps;
    sum = 0.000000;
    {
        double x;
        x = a;
        for (; x < b; x += dx)
        sum += fabs(f(x));
    }
    return dx * sum;
}



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
double __cdecl sin(double _X);

int main(void)
{
    printf("fabs(+3) = %f\n", fabs(3.000000));
    printf("fabs(-3) = %f\n", fabs(-3.000000));
    printf("fabs(-0) = %f\n", fabs(-0.000000));
    printf("fabs(-Inf) = %f\n", fabs(-1.000000f));
    printf("Area under sin(x) in [-PI, PI] = %f\n", integrate(sin, -3.141590, 3.141590, 5101));
}


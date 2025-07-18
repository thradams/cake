struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
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
int __cdecl fetestexcept(int _Flags);
int __cdecl feclearexcept(int _Flags);

void show_fe_exceptions(void)
{
    printf("exceptions raised:");
    if (fetestexcept(8))
    {
        printf(" FE_DIVBYZERO");
    }
    if (fetestexcept(1))
    {
        printf(" FE_INEXACT");
    }
    if (fetestexcept(16))
    {
        printf(" FE_INVALID");
    }
    if (fetestexcept(4))
    {
        printf(" FE_OVERFLOW");
    }
    if (fetestexcept(2))
    {
        printf(" FE_UNDERFLOW");
    }
    feclearexcept(31);
    printf("\n");
}

double __cdecl sqrt(double _X);
double __cdecl nextafter(double _X, double _Y);
double __cdecl pow(double _X, double _Y);

int main(void)
{
    printf("MATH_ERREXCEPT is %s\n", 2 ? "set" : "not set");
    printf("0.0/0.0 = %f\n", 0.000000 / 0.000000);
    show_fe_exceptions();
    printf("1.0/0.0 = %f\n", 1.000000 / 0.000000);
    show_fe_exceptions();
    printf("1.0/10.0 = %f\n", 0.100000);
    show_fe_exceptions();
    printf("sqrt(-1) = %f\n", sqrt(-1));
    show_fe_exceptions();
    printf("DBL_MAX*2.0 = %f\n", inf);
    show_fe_exceptions();
    printf("nextafter(DBL_MIN/pow(2.0,52),0.0) = %.1f\n", nextafter(0.000000 / pow(2.000000, 52), 0.000000));
    show_fe_exceptions();
}


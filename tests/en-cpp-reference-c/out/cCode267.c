struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


double __cdecl acos(double _X);
double __cdecl cos(double _X);

double cos_pi_x_naive(double x)
{
    double pi;

    pi = acos(-1);
    return cos(pi * x);
}

double __cdecl remquo(double _X, double _Y, int * _Z);

double cos_pi_x_smart(double x)
{
    double pi;
    int extremum;
    double rem;

    pi = acos(-1);
    rem = remquo(x, 1, &extremum);
    extremum = (unsigned int)extremum % 2;
    return extremum ? -cos(pi * rem) : cos(pi * rem);
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
int __cdecl feclearexcept(int _Flags);
int __cdecl fetestexcept(int _Flags);
int __cdecl puts(char * _Buffer);

int main(void)
{
    int quo;

    printf("cos(pi * 0.25) = %f\n", cos_pi_x_naive(0.250000));
    printf("cos(pi * 1.25) = %f\n", cos_pi_x_naive(1.250000));
    printf("cos(pi * 1000000000000.25) = %f\n", cos_pi_x_naive(1000000000000.250000));
    printf("cos(pi * 1000000000001.25) = %f\n", cos_pi_x_naive(1000000000001.250000));
    printf("cos(pi * 1000000000000.25) = %f\n", cos_pi_x_smart(1000000000000.250000));
    printf("cos(pi * 1000000000001.25) = %f\n", cos_pi_x_smart(1000000000001.250000));
    feclearexcept(31);
    printf("remquo(+Inf, 1) = %.1f\n", remquo(1.000000f, 1, &quo));
    if (fetestexcept(16))
    {
        puts("    FE_INVALID raised");
    }
}


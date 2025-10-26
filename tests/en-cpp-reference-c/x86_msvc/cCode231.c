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


double __cdecl fabs(double _X);
double f(double);

double integrate(double f(double), double a, double b, unsigned int steps)
{
    double dx;
    double sum;

    dx = (b - a) / steps;
    sum = 0.0;
    {
        double x;

        x = a;
        for (; x < b; x += dx)
        sum += fabs(f(x));
    }
    return dx * sum;
}


static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
double __cdecl sin(double _X);

int main(void)
{
    printf("fabs(+3) = %f\n", fabs(3.0));
    printf("fabs(-3) = %f\n", fabs(-3.0));
    printf("fabs(-0) = %f\n", fabs(-0.0));
    printf("fabs(-Inf) = %f\n", fabs(-inff));
    printf("Area under sin(x) in [-PI, PI] = %f\n", integrate(sin, -3.1415899999999999, 3.1415899999999999, 5101));
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

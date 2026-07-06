/* Cake x64_msvc */

struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

int __cdecl feclearexcept(int _Flags);
double __cdecl sqrt(double _X);
int __cdecl fetestexcept(int _Flags);
double __cdecl frexp(double _X, int * _Y);
double __cdecl fabs(double _X);
double __cdecl scalbn(double _X, int _Y);

double hypot_demo(double a, double b)
{
    int range_problem;
    double result;
    int a_exponent;
    int b_exponent;
    double a_scaled;
    double b_scaled;

    range_problem = 6;
    feclearexcept(6);
    result = sqrt(a * a + b * b);
    if (!fetestexcept(6))
    {
        return result;
    }
    frexp(a, &a_exponent);
    frexp(b, &b_exponent);
    if (a_exponent - b_exponent > 1024)
    {
        return fabs(a) + fabs(b);
    }
    a_scaled = scalbn(a, -a_exponent);
    b_scaled = scalbn(b, -a_exponent);
    result = sqrt(a_scaled * a_scaled + b_scaled * b_scaled);
    return scalbn(result, a_exponent);
}

static int __cdecl printf(char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    printf("hypot(%f, %f) = %f\n", 3.0, 4.0, hypot_demo(3.0, 4.0));
    printf("hypot(%e, %e) = %e\n", 8.9884656743115785e+307, 8.9884656743115785e+307, hypot_demo(8.9884656743115785e+307, 8.9884656743115785e+307));
    return 0;
}

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &_OptionsStorage;
}

static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(__va_start(&_ArgList, _Format)));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

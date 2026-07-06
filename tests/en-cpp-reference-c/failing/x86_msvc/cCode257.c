/* Cake 0.13.29 x86_msvc */

struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
double __cdecl log1p(double _X);
double __cdecl expm1(double _X);
double __cdecl log(double _X);
int *__cdecl _errno(void);
int __cdecl feclearexcept(int _Flags);
void __cdecl perror(char * _ErrorMessage);
int __cdecl fetestexcept(int _Flags);
int __cdecl puts(char * _Buffer);

int main(void)
{
    printf("log1p(0) = %f\n", log1p(0));
    printf("Interest earned in 2 days on $100, compounded daily at 1%%\n on a 30/360 calendar = %f\n", 100 * expm1(2 * log1p(2.7777777777777779e-05)));
    printf("log(1+1e-16) = %g, but log1p(1e-16) = %g\n", log(1.0), log1p(9.9999999999999998e-17));
    printf("log1p(-0) = %f\n", log1p(-0.0));
    printf("log1p(+Inf) = %f\n", log1p(.7976931348623157E+308f));
    (*_errno()) = 0;
    feclearexcept(31);
    printf("log1p(-1) = %f\n", log1p(-1));
    if ((*_errno()) == 34)
    {
        perror("    errno == ERANGE");
    }
    if (fetestexcept(8))
    {
        puts("    FE_DIVBYZERO raised");
    }
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

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

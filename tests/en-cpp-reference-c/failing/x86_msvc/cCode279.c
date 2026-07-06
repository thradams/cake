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
double __cdecl tgamma(double _X);
double __cdecl sqrt(double _X);
double __cdecl acos(double _X);
int *__cdecl _errno(void);
int __cdecl feclearexcept(int _Flags);
void __cdecl perror(char * _ErrorMessage);
int __cdecl fetestexcept(int _Flags);
int __cdecl puts(char * _Buffer);

int main(void)
{
    printf("tgamma(10) = %f, 9!=%f\n", tgamma(10), 362880.0);
    printf("tgamma(0.5) = %f, sqrt(pi) = %f\n", sqrt(acos(-1)), tgamma(0.5));
    printf("tgamma(+Inf) = %f\n", tgamma(.7976931348623157E+308f));
    (*_errno()) = 0;
    feclearexcept(31);
    printf("tgamma(-1) = %f\n", tgamma(-1));
    if ((*_errno()) == 34)
    {
        perror("    errno == ERANGE");
    }
    else
    {
        if ((*_errno()) == 33)
        {
            perror("    errno == EDOM");
        }
    }
    if (fetestexcept(8))
    {
        puts("    FE_DIVBYZERO raised");
    }
    else
    {
        if (fetestexcept(16))
        {
            puts("    FE_INVALID raised");
        }
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

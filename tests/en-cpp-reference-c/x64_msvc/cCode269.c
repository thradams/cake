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

static int __cdecl printf(char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
double __cdecl round(double _X);
long __cdecl lround(double _X);
int __cdecl feclearexcept(int _Flags);
int __cdecl fetestexcept(int _Flags);
int __cdecl puts(char * _Buffer);

int main(void)
{
    printf("round(+2.3) = %+.1f  ", round(2.2999999999999998));
    printf("round(+2.5) = %+.1f  ", round(2.5));
    printf("round(+2.7) = %+.1f\n", round(2.7000000000000002));
    printf("round(-2.3) = %+.1f  ", round(-2.2999999999999998));
    printf("round(-2.5) = %+.1f  ", round(-2.5));
    printf("round(-2.7) = %+.1f\n", round(-2.7000000000000002));
    printf("round(-0.0) = %+.1f\n", round(-0.0));
    printf("round(-Inf) = %+f\n", round(.7976931348623157E+308f));
    printf("lround(+2.3) = %ld  ", lround(2.2999999999999998));
    printf("lround(+2.5) = %ld  ", lround(2.5));
    printf("lround(+2.7) = %ld\n", lround(2.7000000000000002));
    printf("lround(-2.3) = %ld  ", lround(-2.2999999999999998));
    printf("lround(-2.5) = %ld  ", lround(-2.5));
    printf("lround(-2.7) = %ld\n", lround(-2.7000000000000002));
    printf("lround(-0.0) = %ld\n", lround(-0.0));
    printf("lround(-Inf) = %ld\n", lround(.7976931348623157E+308f));
    feclearexcept(31);
    printf("lround(LONG_MAX+1.5) = %ld\n", lround(2147483648.5));
    if (fetestexcept(16))
    {
        puts("    FE_INVALID was raised");
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

    ((void)(__va_start(&_ArgList, _Format)));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

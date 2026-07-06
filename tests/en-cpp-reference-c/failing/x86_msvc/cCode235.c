/* Cake x86_msvc */
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
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
double __cdecl fma(double _X, double _Y, double _Z);
int __cdecl feclearexcept(int _Flags);
int __cdecl fetestexcept(int _Flags);
int __cdecl puts(char * _Buffer);

int main(void)
{
    double in;
    double expr_result;
    double fma_result;
    double high;
    double low;

    in = 0.10000000000000001;
    printf("0.1 double is %.23f (%a)\n", in, in);
    printf("0.1*10 is 1.0000000000000000555112 (0x8.0000000000002p-3), or 1.0 if rounded to double\n");
    expr_result = 0.0;
    printf("0.1 * 10 - 1 = %g : 1 subtracted after intermediate rounding to 1.0\n", expr_result);
    fma_result = fma(0.10000000000000001, 10, -1);
    printf("fma(0.1, 10, -1) = %g (%a)\n", fma_result, fma_result);
    printf("\nin double-double arithmetic, 0.1 * 10 is representable as ");
    high = 1.0;
    low = fma(0.10000000000000001, 10, -high);
    printf("%g + %g\n\n", high, low);
    feclearexcept(31);
    printf("fma(+Inf, 10, -Inf) = %f\n", fma(.7976931348623157E+308f, 10, .7976931348623157E+308f));
    if (fetestexcept(16))
    {
        puts("    FE_INVALID raised");
    }
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

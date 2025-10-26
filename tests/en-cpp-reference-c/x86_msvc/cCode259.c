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


static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
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

    f = 123.45;
    printf("Given the number %.2f or %a in hex,\n", f, f);
    f2 = modf(f, &f3);
    printf("modf() makes %.0f + %.2f\n", f3, f2);
    f2 = frexp(f, &i);
    printf("frexp() makes %f * 2^%d\n", f2, i);
    i = logb(f);
    printf("logb()/logb() make %f * %d^%d\n", f / scalbn(1.0, i), 2, i);
    feclearexcept(31);
    printf("logb(0) = %f\n", logb(0));
    if (fetestexcept(8))
    {
        puts("    FE_DIVBYZERO raised");
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

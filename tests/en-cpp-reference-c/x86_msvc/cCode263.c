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


int __cdecl fesetround(int _Round);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
double __cdecl nearbyint(double _X);

int main(void)
{
    fesetround(0);
    printf("rounding to nearest:\nnearbyint(+2.3) = %+.1f  ", nearbyint(2.2999999999999998));
    printf("nearbyint(+2.5) = %+.1f  ", nearbyint(2.5));
    printf("nearbyint(+3.5) = %+.1f\n", nearbyint(3.5));
    printf("nearbyint(-2.3) = %+.1f  ", nearbyint(-2.2999999999999998));
    printf("nearbyint(-2.5) = %+.1f  ", nearbyint(-2.5));
    printf("nearbyint(-3.5) = %+.1f\n", nearbyint(-3.5));
    fesetround(256);
    printf("rounding down: \nnearbyint(+2.3) = %+.1f  ", nearbyint(2.2999999999999998));
    printf("nearbyint(+2.5) = %+.1f  ", nearbyint(2.5));
    printf("nearbyint(+3.5) = %+.1f\n", nearbyint(3.5));
    printf("nearbyint(-2.3) = %+.1f  ", nearbyint(-2.2999999999999998));
    printf("nearbyint(-2.5) = %+.1f  ", nearbyint(-2.5));
    printf("nearbyint(-3.5) = %+.1f\n", nearbyint(-3.5));
    printf("nearbyint(-0.0) = %+.1f\n", nearbyint(-0.0));
    printf("nearbyint(-Inf) = %+.1f\n", nearbyint(-inff));
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

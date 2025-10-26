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
int __cdecl __fpe_flt_rounds(void);

int main(void)
{
    printf("DECIMAL_DIG     = %d\n", 17);
    printf("FLT_DECIMAL_DIG = %d\n", 9);
    printf("FLT_RADIX       = %d\n", 2);
    printf("FLT_MIN         = %e\n", 0.000000f);
    printf("FLT_MAX         = %e\n", 340282346638528859811704183484516925440.000000f);
    printf("FLT_EPSILON     = %e\n", 0.000000f);
    printf("FLT_DIG         = %d\n", 6);
    printf("FLT_MANT_DIG    = %d\n", 24);
    printf("FLT_MIN_EXP     = %d\n", -125);
    printf("FLT_MIN_10_EXP  = %d\n", -37);
    printf("FLT_MAX_EXP     = %d\n", 128);
    printf("FLT_MAX_10_EXP  = %d\n", 38);
    printf("FLT_ROUNDS      = %d\n", (__fpe_flt_rounds()));
    printf("FLT_EVAL_METHOD = %d\n", 2);
    printf("FLT_HAS_SUBNORM = %d\n", 1);
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

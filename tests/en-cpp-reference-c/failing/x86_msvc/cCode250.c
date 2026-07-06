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
short __cdecl _fdclass(float _X);
short __cdecl _dclass(double _X);
short __cdecl _ldclass(long double _X);

int main(void)
{
    printf("isnan(NAN)         = %d\n", ((1 ? _fdclass(nan.0f) : (4 == 4 ? 102 : 8 == 8 ? 100 : 108) == 100 ? _dclass((double)(-(float)(((float)(1.0000000000000001e+300 * 1.0000000000000001e+300)) * 0.0))) : _ldclass((long double)(-(float)(((float)(1.0000000000000001e+300 * 1.0000000000000001e+300)) * 0.0)))) == 2));
    printf("isnan(INFINITY)    = %d\n", ((1 ? _fdclass(.7976931348623157E+308f) : (4 == 4 ? 102 : 8 == 8 ? 100 : 108) == 100 ? _dclass((double)((float)1.0000000000000001e+300)) : _ldclass((long double)((float)1.0000000000000001e+300))) == 2));
    printf("isnan(0.0)         = %d\n", ((0 ? _fdclass((float)0.0) : 1 ? _dclass(0.0) : _ldclass((long double)0.0)) == 2));
    printf("isnan(DBL_MIN/2.0) = %d\n", ((0 ? _fdclass((float)(2.2250738585072014e-308 / 2.0)) : 1 ? _dclass(1.1125369292536007e-308) : _ldclass((long double)(2.2250738585072014e-308 / 2.0))) == 2));
    printf("isnan(0.0 / 0.0)   = %d\n", ((0 ? _fdclass((float)(0.0 / 0.0)) : 1 ? _dclass(-nan(ind).0) : _ldclass((long double)(0.0 / 0.0))) == 2));
    printf("isnan(Inf - Inf)   = %d\n", ((1 ? _fdclass(-nan(ind).0f) : (4 == 4 ? 102 : 8 == 8 ? 100 : 108) == 100 ? _dclass((double)(((float)1.0000000000000001e+300) - ((float)1.0000000000000001e+300))) : _ldclass((long double)(((float)1.0000000000000001e+300) - ((float)1.0000000000000001e+300)))) == 2));
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

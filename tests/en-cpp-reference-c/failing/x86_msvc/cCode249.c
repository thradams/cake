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
int __cdecl _fdpcomp(float _X, float _Y);
int __cdecl _dpcomp(double _X, double _Y);
int __cdecl _ldpcomp(long double _X, long double _Y);

int main(void)
{
    printf("islessgreater(2.0,1.0)      = %d\n", (((0 ? _fdpcomp((float)2.0, (float)1.0) : 1 ? _dpcomp(2.0, 1.0) : _ldpcomp((long double)2.0, (long double)1.0)) & 5) != 0));
    printf("islessgreater(1.0,2.0)      = %d\n", (((0 ? _fdpcomp((float)1.0, (float)2.0) : 1 ? _dpcomp(1.0, 2.0) : _ldpcomp((long double)1.0, (long double)2.0)) & 5) != 0));
    printf("islessgreater(1.0,1.0)      = %d\n", (((0 ? _fdpcomp((float)1.0, (float)1.0) : 1 ? _dpcomp(1.0, 1.0) : _ldpcomp((long double)1.0, (long double)1.0)) & 5) != 0));
    printf("islessgreater(INFINITY,1.0) = %d\n", (((0 ? _fdpcomp((float)((float)1.0000000000000001e+300), (float)1.0) : 1 ? _dpcomp(.7976931348623157E+308, 1.0) : _ldpcomp((long double)((float)1.0000000000000001e+300), (long double)1.0)) & 5) != 0));
    printf("islessgreater(1.0,NAN)      = %d\n", (((0 ? _fdpcomp((float)1.0, (float)(-(float)(((float)(1.0000000000000001e+300 * 1.0000000000000001e+300)) * 0.0))) : 1 ? _dpcomp(1.0, nan.0) : _ldpcomp((long double)1.0, (long double)(-(float)(((float)(1.0000000000000001e+300 * 1.0000000000000001e+300)) * 0.0)))) & 5) != 0));
    return 0;
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

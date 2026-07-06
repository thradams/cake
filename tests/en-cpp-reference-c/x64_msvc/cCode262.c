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

double __cdecl nan(char * _X);
void *__cdecl memcpy(void * _Dst, void * _Src, unsigned long long _Size);
static int __cdecl printf(char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    double f1;
    unsigned long long f1n;
    double f2;
    unsigned long long f2n;
    double f3;
    unsigned long long f3n;

    f1 = nan("1");
    memcpy(&f1n, &f1, 8ULL);
    printf("nan(\"1\")   = %f (%llx)\n", f1, f1n);
    f2 = nan("2");
    memcpy(&f2n, &f2, 8ULL);
    printf("nan(\"2\")   = %f (%llx)\n", f2, f2n);
    f3 = nan("0xF");
    memcpy(&f3n, &f3, 8ULL);
    printf("nan(\"0xF\") = %f (%llx)\n", f3, f3n);
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

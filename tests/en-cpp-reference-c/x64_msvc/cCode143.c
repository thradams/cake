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

int main(void)
{
    short x;

    printf("sizeof(float)          = %zu\n", 4ULL);
    printf("sizeof(void(*)(void))  = %zu\n", 8ULL);
    printf("sizeof(char[10])       = %zu\n", 10ULL);
    printf("sizeof 'a'             = %zu\n", 4ULL);
    printf("sizeof &main           = %zu\n", 8ULL);
    printf("sizeof \"hello\"         = %zu\n", 6ULL);
    printf("sizeof x               = %zu\n", 2ULL);
    printf("sizeof (x+1)           = %zu\n", 4ULL);
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

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};




int __cdecl __stdio_common_vfprintf(unsigned __int64 _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

static unsigned __int64 __ck__OptionsStorage1;
inline unsigned __int64 *__cdecl __local_stdio_printf_options(void)
{
    return &__ck__OptionsStorage1;
}
inline int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
inline int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}
long long __cdecl strtoimax(char * _String, char ** _EndPtr, int _Radix);
int *__cdecl _errno(void);
char *__cdecl strerror(int _ErrorMessage);

int main(void)
{
    char * endptr;

    printf("%ld\n", strtoimax(" -123junk", &endptr, 10));
    printf("%ld\n", strtoimax("11111111", &endptr, 2));
    printf("%ld\n", strtoimax("XyZ", &endptr, 36));
    printf("%ld\n", strtoimax("010", &endptr, 0));
    printf("%ld\n", strtoimax("10", &endptr, 0));
    printf("%ld\n", strtoimax("0x10", &endptr, 0));
    (*_errno()) = 0;
    printf("%ld\n", strtoimax("9223372036854775808", &endptr, 10));
    printf("%s\n", strerror((*_errno())));
    return 0;
}


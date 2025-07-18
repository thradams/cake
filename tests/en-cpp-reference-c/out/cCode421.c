struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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

int main(void)
{
    printf("CHAR_BIT       = %d\n", 8);
    printf("MB_LEN_MAX     = %d\n\n", 5);
    printf("CHAR_MIN       = %+d\n", -128);
    printf("CHAR_MAX       = %+d\n", 127);
    printf("SCHAR_MIN      = %+d\n", -128);
    printf("SCHAR_MAX      = %+d\n", 127);
    printf("UCHAR_MAX      = %u\n\n", 255);
    printf("SHRT_MIN       = %+d\n", -32768);
    printf("SHRT_MAX       = %+d\n", 32767);
    printf("USHRT_MAX      = %u\n\n", 65535);
    printf("INT_MIN        = %+d\n", -2147483648);
    printf("INT_MAX        = %+d\n", 2147483647);
    printf("UINT_MAX       = %u\n\n", 4294967295LL);
    printf("LONG_MIN       = %+ld\n", -2147483648L);
    printf("LONG_MAX       = %+ld\n", 2147483647L);
    printf("ULONG_MAX      = %lu\n\n", 4294967295UL);
    printf("LLONG_MIN      = %+lld\n", -9223372036854775808LL);
    printf("LLONG_MAX      = %+lld\n", 9223372036854775807LL);
    printf("ULLONG_MAX     = %llu\n\n", 18446744073709551615ULL);
    printf("PTRDIFF_MIN    = %td\n", -2147483648);
    printf("PTRDIFF_MAX    = %+td\n", 2147483647);
    printf("SIZE_MAX       = %zu\n", 4294967295U);
    printf("SIG_ATOMIC_MIN = %+jd\n", -2147483648LL);
    printf("SIG_ATOMIC_MAX = %+jd\n", 2147483647LL);
    printf("WCHAR_MIN      = %+jd\n", 0LL);
    printf("WCHAR_MAX      = %+jd\n", 65535LL);
    printf("WINT_MIN       = %jd\n", 0LL);
    printf("WINT_MAX       = %jd\n", 65535LL);
}


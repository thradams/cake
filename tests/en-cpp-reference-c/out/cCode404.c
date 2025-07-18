struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


int _Avx2WmemEnabledWeakValue = 0;


int __cdecl __stdio_common_vfwprintf(unsigned __int64 _Options, struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

static unsigned __int64 __ck__OptionsStorage1;
inline unsigned __int64 *__cdecl __local_stdio_printf_options(void)
{
    return &__ck__OptionsStorage1;
}
inline int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfwprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
inline int __cdecl wprintf(unsigned short * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfwprintf_l((__acrt_iob_func(1)), _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}
long long __cdecl wcstoimax(unsigned short * _String, unsigned short ** _EndPtr, int _Radix);
int *__cdecl _errno(void);
char *__cdecl strerror(int _ErrorMessage);

int main(void)
{
    unsigned short * endptr;

    wprintf(L"%ld\n", wcstoimax(L" -123junk", &endptr, 10));
    wprintf(L"%ld\n", wcstoimax(L"11111111", &endptr, 2));
    wprintf(L"%ld\n", wcstoimax(L"XyZ", &endptr, 36));
    wprintf(L"%ld\n", wcstoimax(L"010", &endptr, 0));
    wprintf(L"%ld\n", wcstoimax(L"10", &endptr, 0));
    wprintf(L"%ld\n", wcstoimax(L"0x10", &endptr, 0));
    (*_errno()) = 0;
    wprintf(L"%ld\n", wcstoimax(L"9223372036854775808", &endptr, 10));
    wprintf(L"%s\n", strerror((*_errno())));
}


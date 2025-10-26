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


__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;
static int __cdecl wprintf(unsigned short * _Format, ...);
static int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfwprintf(unsigned long long _Options, struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
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



static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__c0__OptionsStorage;
}

static int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfwprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl wprintf(unsigned short * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vfwprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

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
int __cdecl wcscoll(unsigned short * _String1, unsigned short * _String2);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void try_compare(unsigned short * p1, unsigned short * p2)
{
    if (wcscoll(p1, p2) < 0)
    {
        printf("%ls before %ls\n", p1, p2);
    }
    else
    {
        printf("%ls before %ls\n", p2, p1);
    }
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
char *__cdecl setlocale(int _Category, char * _Locale);

int main(void)
{
    setlocale(0, "en_US.utf8");
    printf("In the American locale: ");
    try_compare(L"hrnec", L"chrt");
    setlocale(1, "cs_CZ.utf8");
    printf("In the Czech locale: ");
    try_compare(L"hrnec", L"chrt");
    setlocale(1, "en_US.utf8");
    printf("In the American locale: ");
    try_compare(L"\xc3\xa5r", L"\xc3\xa4ngel");
    setlocale(1, "sv_SE.utf8");
    printf("In the Swedish locale: ");
    try_compare(L"\xc3\xa5r", L"\xc3\xa4ngel");
}



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
int __cdecl wcsncmp(unsigned short * _String1, unsigned short * _String2, unsigned int _MaxCount);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void demo(unsigned short * lhs, unsigned short * rhs, int sz)
{
    int rc;

    rc = wcsncmp(lhs, rhs, sz);
    if (rc == 0)
    {
        printf("First %d characters of [%ls] equal [%ls]\n", sz, lhs, rhs);
    }
    else
    {
        if (rc < 0)
        {
            printf("First %d characters of [%ls] precede [%ls]\n", sz, lhs, rhs);
        }
        else
        {
            if (rc > 0)
            {
                printf("First %d characters of [%ls] follow [%ls]\n", sz, lhs, rhs);
            }
        }
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
    unsigned short * str1;
    unsigned short * str2;

    str1 = L"\xec\x95\x88\xeb\x85\x95\xed\x95\x98\xec\x84\xb8\xec\x9a\x94";
    str2 = L"\xec\x95\x88\xeb\x85\x95\xed\x9e\x88 \xea\xb0\x80\xec\x8b\xad\xec\x8b\x9c\xec\x98\xa4";
    setlocale(0, "en_US.utf8");
    demo(str1, str2, 5);
    demo(str2, str1, 8);
    demo(str1, str2, 2);
}



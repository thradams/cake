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
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl iswctype(unsigned short _C, unsigned short _Type);
unsigned short __cdecl wctype(char * name);

void test(unsigned short a3, unsigned short u3, unsigned short j3)
{
    printf("\t '%lc'  '%lc' '%lc'\n", a3, u3, j3);
    printf("iswdigit: %d    %d    %d\n", !!(iswctype(a3, 4)), !!(iswctype(u3, 4)), !!(iswctype(j3, 4)));
    printf("jdigit:   %d    %d    %d\n", !!iswctype(a3, wctype("jdigit")), !!iswctype(u3, wctype("jdigit")), !!iswctype(j3, wctype("jdigit")));
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
int __cdecl puts(char * _Buffer);

int main(void)
{
    unsigned short a3;
    unsigned short u3;
    unsigned short j3;

    a3 = 51;
    u3 = 19977;
    j3 = 65299;
    setlocale(0, "en_US.utf8");
    puts("In American locale:");
    test(a3, u3, j3);
    setlocale(0, "ja_JP.utf8");
    puts("\nIn Japanese locale:");
    test(a3, u3, j3);
}



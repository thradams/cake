/* Cake 0.12.26 x86_msvc */
struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;
int __cdecl wctob(unsigned short _WCh);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void try_narrowing(unsigned short c)
{
    int cn;

    cn = wctob(c);
    if (cn != -1)
    {
        printf("%#x narrowed to %#x\n", c, cn);
    }
    else
    {
        printf("%#x could not be narrowed\n", c);
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
int __cdecl puts(char * _Buffer);

int main(void)
{
    char * utf_locale_present;
    char * tis_locale_present;

    utf_locale_present = setlocale(0, "th_TH.utf8");
    ;
    puts("In Thai UTF-8 locale:");
    try_narrowing(97);
    try_narrowing(3675);
    tis_locale_present = setlocale(0, "th_TH.tis620");
    ;
    puts("In Thai TIS-620 locale:");
    try_narrowing(97);
    try_narrowing(3675);
}



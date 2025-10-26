/* Cake 0.12.26 x86_msvc */
struct _iobuf {
    void * _Placeholder;
};

struct _Mbstatet {
    unsigned long _Wchar;
    unsigned short _Byte;
    unsigned short _State;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;
void *__cdecl memset(void * _Dst, int _Val, unsigned int _Size);
unsigned int __cdecl wcsrtombs(char * _Dest, unsigned short ** _PSource, unsigned int _Count, struct _Mbstatet * _State);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void print_wide(unsigned short * wstr)
{
    struct _Mbstatet  state;
    unsigned int len;
    char mbstr[];

    memset(&state, 0, 8);
    len = 1 + wcsrtombs(0, &wstr, 0, &state);
    wcsrtombs(mbstr, &wstr, len, &state);
    printf("Multibyte string: %s\n", mbstr);
    printf("Length, including '\\0': %zu\n", len);
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
    print_wide(L"z\u00df\u6c34\U0001f34c");
}



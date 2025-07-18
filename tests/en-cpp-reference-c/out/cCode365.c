struct _Mbstatet {
    unsigned long _Wchar;
    unsigned short _Byte;
    unsigned short _State;
};

struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


int _Avx2WmemEnabledWeakValue = 0;
void *__cdecl memset(void * _Dst, int _Val, unsigned int _Size);
unsigned int __cdecl mbsrtowcs(unsigned short * _Dest, char ** _PSrc, unsigned int _Count, struct _Mbstatet * _State);


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

void print_as_wide(char * mbstr)
{
    struct _Mbstatet  state;
    unsigned int len;
    unsigned short wstr[];

    memset(&state, 0, 8U);
    len = 1 + mbsrtowcs(0U, &mbstr, 0, &state);
    mbsrtowcs(&wstr[0], &mbstr, len, &state);
    wprintf(L"Wide string: %ls \n", wstr);
    wprintf(L"The length, including L'\\0': %zu\n", len);
}

char *__cdecl setlocale(int _Category, char * _Locale);

int main(void)
{
    setlocale(0, "en_US.utf8");
    print_as_wide("z\u00df\u6c34\U0001f34c");
}


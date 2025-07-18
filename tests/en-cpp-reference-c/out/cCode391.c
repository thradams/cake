struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


int _Avx2WmemEnabledWeakValue = 0;
int __cdecl wcscmp(unsigned short * _String1, unsigned short * _String2);
char *__cdecl setlocale(int _Category, char * _Locale);


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

void demo(unsigned short * lhs, unsigned short * rhs)
{
    int rc;
    char * rel;

    rc = wcscmp(lhs, rhs);
    rel = rc < 0 ? "precedes" : rc > 0 ? "follows" : "equals";
    setlocale(0, "en_US.utf8");
    printf("[%ls] %s [%ls]\n", lhs, rel, rhs);
}

int main(void)
{
    unsigned short * string;

    string = L"\xe3\x81\xa9\xe3\x81\x86\xe3\x82\x82\xe3\x81\x82\xe3\x82\x8a\xe3\x81\x8c\xe3\x81\xa8\xe3\x81\x86\xe3\x81\x94\xe3\x81\x96\xe3\x81\x84\xe3\x81\xbe\xe3\x81\x99";
    demo(string, L"\xe3\x81\xa9\xe3\x81\x86\xe3\x82\x82");
    demo(string, L"\xe5\x8a\xa9\xe3\x81\x8b\xe3\x81\xa3\xe3\x81\x9f");
    demo(string + 9, L"\xe3\x81\x82\xe3\x82\x8a\xe3\x81\x8c\xe3\x81\xa8\xe3\x81\x86\xe3\x81\x94\xe3\x81\x96\xe3\x81\x84\xe3\x81\xbe\xe3\x81\x99" + 6);
}


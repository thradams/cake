struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


int _Avx2WmemEnabledWeakValue = 0;
unsigned short __cdecl btowc(int _Ch);


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

void try_widen(unsigned char c)
{
    unsigned short w;

    w = btowc(c);
    if (w != 65535)
    {
        printf("The single-byte character %#x widens to %#x\n", c, w);
    }
    else
    {
        printf("The single-byte character %#x failed to widen\n", c);
    }
}

char *__cdecl setlocale(int _Category, char * _Locale);

int main(void)
{
    char * loc;

    loc = setlocale(0, "lt_LT.iso88594");
    ;
    printf("In Lithuanian ISO-8859-4 locale:\n");
    try_widen(65);
    try_widen(223);
    try_widen(249);
    setlocale(0, "lt_LT.utf8");
    printf("In Lithuanian UTF-8 locale:\n");
    try_widen(65);
    try_widen(223);
    try_widen(249);
}


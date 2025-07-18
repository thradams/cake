struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


static void _cake_memcpy(void * dest, const void * src, unsigned long n)
{
  char *csrc = (char *)src;
  char *cdest = (char *)dest;
  for (int i=0; i<n; i++) cdest[i] = csrc[i]; 
}

int _Avx2WmemEnabledWeakValue = 0;
char *__cdecl setlocale(int _Category, char * _Locale);
unsigned short __cdecl towctrans(unsigned short c, unsigned short value);
unsigned short __cdecl wctrans(char * name);


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

int main(void)
{
    unsigned short kana[13];
    unsigned int sz;
    unsigned short hira[];

    setlocale(0, "ja_JP.UTF-8");
    _cake_memcpy(kana, L"\xe3\x83\x92\xe3\x83\xa9\xe3\x82\xac\xe3\x83\x8a", 13);
    sz = 13U;
    {
        unsigned int n;
        n = 0;
        for (; n < sz; ++n)
        hira[n] = towctrans(kana[n], wctrans("tojhira"));
    }
    printf("katakana characters %ls are %ls in hiragana\n", kana, hira);
}


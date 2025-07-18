struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


static void _cake_memcpy(void * dest, const void * src, unsigned long n)
{
  char *csrc = (char *)src;
  char *cdest = (char *)dest;
  for (int i=0; i<n; i++) cdest[i] = csrc[i]; 
}

int _Avx2WmemEnabledWeakValue = 0;
unsigned int __cdecl wcsspn(unsigned short * _String, unsigned short * _Control);
char *__cdecl setlocale(int _Category, char * _Locale);


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

int main(void)
{
    unsigned short dest[21];
    unsigned short src[16];
    unsigned int len;

    _cake_memcpy(dest, L"\xe7\x99\xbd\xe7\x8c\xab \xe9\xbb\x91\xe7\x8b\x97 \xe7\x94\xb2\xe8\x99\xab", 21);
    _cake_memcpy(src, L" \xe7\x8b\x97\xe7\x8c\xab \xe7\x99\xbd\xe9\xbb\x91 ", 16);
    len = wcsspn(dest, src);
    dest[len] = 0;
    setlocale(0, "en_US.utf8");
    wprintf(L"The length of maximum initial segment is %td.\n"L"The segment is \"%ls\".\n", len, dest);
}


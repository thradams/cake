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

inline unsigned short *__cdecl wmemchr(unsigned short * _S, unsigned short _C, unsigned int _N)
{
    unsigned int __count;

    __count = 0;
    for (; __count < _N; ++__count)
    {
        if (_S[__count] == _C)
        {
            return (unsigned short *)&_S[__count];
        }
    }
    return 0U;
}
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

int main(void)
{
    unsigned short str[65];
    unsigned int sz;
    unsigned short target;
    unsigned short * result;

    _cake_memcpy(str, L"\xe8\xaf\xba\xe4\xb8\x8d\xe8\xbd\xbb\xe4\xbf\xa1\xef\xbc\x8c\xe6\x95\x85\xe4\xba\xba\xe4\xb8\x8d\xe8\xb4\x9f\xe6\x88\x91\0\xe8\xaf\xba\xe4\xb8\x8d\xe8\xbd\xbb\xe8\xae\xb8\xef\xbc\x8c\xe6\x95\x85\xe6\x88\x91\xe4\xb8\x8d\xe8\xb4\x9f\xe4\xba\xba\xe3\x80\x82", 65);
    sz = 65U;
    target = -29768;
    result = wmemchr(str, target, sz);
    if (result)
    {
        setlocale(0, "en_US.utf8");
        printf("Found '%lc' at position %td\n", target, result - str);
    }
}


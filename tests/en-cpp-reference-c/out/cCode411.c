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

void *__cdecl memcpy(void * _Dst, void * _Src, unsigned int _Size);
inline unsigned short *__cdecl wmemcpy(unsigned short * _S1, unsigned short * _S2, unsigned int _N)
{
    return (unsigned short *)memcpy(_S1, _S2, _N * 2U);
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
    unsigned short from1[19];
    unsigned int sz1;
    unsigned short from2[9];
    unsigned int sz2;
    unsigned short to[];

    _cake_memcpy(from1, L"\xe0\xa4\xa8\xe0\xa4\xae\xe0\xa4\xb8\xe0\xa5\x8d\xe0\xa4\xa4\xe0\xa5\x87", 19);
    sz1 = 19U;
    _cake_memcpy(from2, L"\xd4\xb2\xd5\xa1\xd6\x80\xd6\x87", 9);
    sz2 = 9U;
    wmemcpy(to, from1, sz1);
    wmemcpy(to + sz1, from2, sz2);
    setlocale(0, "en_US.utf8");
    printf("Wide array contains: ");
    {
        unsigned int n;
        n = 0;
        for (; n < 0 / 2U; ++n)
        if (to[n])
        {
            printf("%lc", to[n]);
        }
        else
        {
            printf("\\0");
        }
    }
    printf("\n");
}


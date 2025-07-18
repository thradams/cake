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
unsigned short *__cdecl wcsncpy(unsigned short * _Destination, unsigned short * _Source, unsigned int _Count);
int __cdecl puts(char * _Buffer);
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
    unsigned short src[7];
    unsigned short dest[6];
    long dest_size;

    _cake_memcpy(src, L"\xe3\x82\x8f\xe3\x82\x90", 7);
    dest[0] = 12354;
    dest[1] = 12356;
    dest[2] = 12358;
    dest[3] = 12360;
    dest[4] = 12362;
    dest[5] = 0;
    wcsncpy(dest, src, 4);
    puts("The contents of dest are: ");
    setlocale(0, "en_US.utf8");
    dest_size = 6U;
    {
        unsigned short * p;
        p = dest;
        for (; p - dest != 6L; ++p)
        {
            *p ? printf("%lc ", *p) : printf("\\0 ");
        }
    }
}


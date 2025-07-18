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

char *__cdecl strncpy(char * _Destination, char * _Source, unsigned int _Count);


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
    char src[3];
    char dest[6];
    char dest2[2];

    _cake_memcpy(src, "hi", 3);
    _cake_memcpy(dest, "abcdef", 6);
    strncpy(dest, src, 5);
    printf("strncpy(dest, src, 5) to a 6-byte dest gives : ");
    {
        unsigned int n;
        n = 0;
        for (; n < 6U; ++n)
        {
            char c;

            c = dest[n];
            c ? printf("'%c' ", c) : printf("'\\0' ");
        }
    }
    printf("\nstrncpy(dest2, src, 2) to a 2-byte dst gives : ");
    strncpy(dest2, src, 2);
    {
        unsigned int n;
        n = 0;
        for (; n < 2U; ++n)
        {
            char c;

            c = dest2[n];
            c ? printf("'%c' ", c) : printf("'\\0' ");
        }
    }
    printf("\n");
}


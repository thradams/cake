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
unsigned short *__cdecl wcsrchr(unsigned short * _Str, unsigned short _Ch);
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
int __cdecl puts(char * _Buffer);

int main(void)
{
    unsigned short arr[25];
    unsigned short * cat;
    unsigned short * dog;

    _cake_memcpy(arr, L"\xe7\x99\xbd\xe7\x8c\xab \xe9\xbb\x92\xe7\x8c\xab \xd0\xba\xd0\xbe\xd1\x88\xd0\xba\xd0\xb8", 25);
    cat = wcsrchr(arr, 29483);
    dog = wcsrchr(arr, 29356);
    setlocale(0, "en_US.utf8");
    cat ? printf("The character \xe7\x8c\xab found at position %td\n", cat - arr) : puts("The character \xe7\x8c\xab not found");
    dog ? printf("The character \xe7\x8a\xac found at position %td\n", dog - arr) : puts("The character \xe7\x8a\xac not found");
}


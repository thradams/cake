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
unsigned short *__cdecl wcstok(unsigned short * _String, unsigned short * _Delimiter, unsigned short ** _Context);
int __cdecl puts(char * _Buffer);

int main(void)
{
    unsigned short input[26];
    unsigned short * buffer;
    unsigned short * token;

    _cake_memcpy(input, L"A bird came down the walk", 26);
    printf("Parsing the input string '%ls'\n", input);
    token = wcstok(input, L" ", &buffer);
    while (token)
    {
        printf("%ls\n", token);
        token = wcstok(0U, L" ", &buffer);
    }
    printf("Contents of the input string now: '");
    {
        unsigned int n;
        n = 0;
        for (; n < 26U; ++n)
        input[n] ? printf("%lc", input[n]) : printf("\\0");
    }
    puts("'");
}


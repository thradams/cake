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
char *__cdecl strtok(char * _String, char * _Delimiter);
int __cdecl puts(char * _Buffer);
int __cdecl putchar(int _Character);
int __cdecl fputs(char * _Buffer, struct _iobuf * _Stream);

int main(void)
{
    char input[26];
    char * token;

    _cake_memcpy(input, "A bird came down the walk", 26);
    printf("Parsing the input string '%s'\n", input);
    token = strtok(input, " ");
    while (token)
    {
        puts(token);
        token = strtok(0U, " ");
    }
    printf("Contents of the input string now: '");
    {
        unsigned int n;
        n = 0;
        for (; n < 26U; ++n)
        input[n] ? putchar(input[n]) : fputs("\\0", (__acrt_iob_func(1)));
    }
    puts("'");
}


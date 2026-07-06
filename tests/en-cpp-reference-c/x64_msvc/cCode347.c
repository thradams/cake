/* Cake x64_msvc */

struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

static void memcpy2(void * dest, const void * src, unsigned long long n);

static int __cdecl printf(char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
char *__cdecl strtok(char * _String, char * _Delimiter);
int __cdecl puts(char * _Buffer);
int __cdecl putchar(int _Character);
int __cdecl fputs(char * _Buffer, struct _iobuf * _Stream);

int main(void)
{
    char input[26];
    char * token;

    memcpy2(input, "A bird came down the walk", 26);
    printf("Parsing the input string '%s'\n", input);
    token = strtok(input, " ");
    while (token)
    {
        puts(token);
        token = strtok(0, " ");
    }
    printf("Contents of the input string now: '");
    {
        unsigned long long n;

        n = 0;
        for (; n < 26ULL; ++n)
        {
            input[n] ? putchar(input[n]) : fputs("\\0", (__acrt_iob_func(1)));
        }
    }
    puts("'");
}

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &_OptionsStorage;
}

static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(__va_start(&_ArgList, _Format)));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

static void memcpy2(void * dest, const void * src, unsigned long long n)
{
    char *csrc;
    char *cdest;
    unsigned long long i; 

    csrc = (char *)src;
    cdest = (char *)dest;
    for (i = 0; i < n; i++)
    {
       cdest[i] = csrc[i]; 
    }
}


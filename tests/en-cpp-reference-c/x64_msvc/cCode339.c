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

char *__cdecl strncpy(char * _Destination, char * _Source, unsigned long long _Count);
static int __cdecl printf(char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    char src[3];
    char dest[6];
    char dest2[2];

    memcpy2(src, "hi", 3);
    memcpy2(dest, "abcdef", 6);
    strncpy(dest, src, 5);
    printf("strncpy(dest, src, 5) to a 6-byte dest gives : ");
    {
        unsigned long long n;

        n = 0;
        for (; n < 6ULL; ++n)
        {
            char c;

            c = dest[n];
            c ? printf("'%c' ", c) : printf("'\\0' ");
        }
    }
    printf("\nstrncpy(dest2, src, 2) to a 2-byte dst gives : ");
    strncpy(dest2, src, 2);
    {
        unsigned long long n;

        n = 0;
        for (; n < 2ULL; ++n)
        {
            char c;

            c = dest2[n];
            c ? printf("'%c' ", c) : printf("'\\0' ");
        }
    }
    printf("\n");
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


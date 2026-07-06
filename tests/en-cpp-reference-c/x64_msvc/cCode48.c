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

static void memcpy(void * dest, const void * src, unsigned long long n);

char *__cdecl setlocale(int _Category, char * _Locale);
static int __cdecl sscanf(char * _Buffer, char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vsscanf_l(char * _Buffer, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vsscanf(unsigned long long _Options, char * _Buffer, unsigned long long _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_scanf_options(void);
static unsigned long long _OptionsStorage;
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage2;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    int i;
    int j;
    float x;
    float y;
    char str1[10];
    char str2[4];
    unsigned short warr[2];
    char input[40];
    int ret;

    setlocale(0, "en_US.utf8");
    memcpy(input, "25 54.32E-1 Thompson 56789 0123 56\xc3\x9f\xe6\xb0\xb4", 40);
    ret = sscanf(input, "%d%f%9s%2d%f%*d %3[0-9]%2lc", &i, &x, str1, &j, &y, str2, warr);
    printf("Converted %d fields:\ni = %d\nx = %f\nstr1 = %s\nj = %d\ny = %f\nstr2 = %s\nwarr[0] = U+%x\nwarr[1] = U+%x\n", ret, i, x, str1, j, y, str2, warr[0], warr[1]);
}

static unsigned long long *__cdecl __local_stdio_scanf_options(void)
{
    return &_OptionsStorage;
}

static int __cdecl _vsscanf_l(char * _Buffer, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vsscanf((*__local_stdio_scanf_options()), _Buffer, 18446744073709551615ULL, _Format, _Locale, _ArgList);
}

static int __cdecl sscanf(char * _Buffer, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(__va_start(&_ArgList, _Format)));
    _Result = _vsscanf_l(_Buffer, _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &_OptionsStorage2;
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

static void memcpy(void * dest, const void * src, unsigned long long n)
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


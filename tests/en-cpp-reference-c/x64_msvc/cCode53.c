/* Cake x64_msvc */

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};

static void memcpy2(void * dest, const void * src, unsigned long long n);
__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;

char *__cdecl setlocale(int _Category, char * _Locale);
static int __cdecl swprintf(unsigned short * _Buffer, unsigned long long _BufferCount, unsigned short * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vswprintf_c_l(unsigned short * _Buffer, unsigned long long _BufferCount, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vswprintf(unsigned long long _Options, unsigned short * _Buffer, unsigned long long _BufferCount, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
static int __cdecl wprintf(unsigned short * _Format, ...);
static int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfwprintf(unsigned long long _Options, struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    char narrow_str[5];
    unsigned short warr[29];

    memcpy2(narrow_str, "z\u00df\u6c34\U0001f34c", 5);
    setlocale(0, "en_US.utf8");
    swprintf(warr, 29ULL, L"Converted from UTF-8: '%s'", narrow_str);
    wprintf(L"%ls\n", warr);
}

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &_OptionsStorage;
}

static int __cdecl _vswprintf_c_l(unsigned short * _Buffer, unsigned long long _BufferCount, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    int _Result;

    _Result = __stdio_common_vswprintf((*__local_stdio_printf_options()), _Buffer, _BufferCount, _Format, _Locale, _ArgList);
    return _Result < 0 ? -1 : _Result;
}

static int __cdecl swprintf(unsigned short * _Buffer, unsigned long long _BufferCount, unsigned short * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(__va_start(&_ArgList, _Format)));
    _Result = _vswprintf_c_l(_Buffer, _BufferCount, _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

static int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfwprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl wprintf(unsigned short * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(__va_start(&_ArgList, _Format)));
    _Result = _vfwprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
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


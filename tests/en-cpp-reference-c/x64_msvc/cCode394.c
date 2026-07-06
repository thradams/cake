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

unsigned long long __cdecl wcscspn(unsigned short * _String, unsigned short * _Control);
char *__cdecl setlocale(int _Category, char * _Locale);
static int __cdecl wprintf(unsigned short * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfwprintf(unsigned long long _Options, struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    unsigned short dest[9];
    unsigned short * src;
    unsigned long long len;

    memcpy2(dest, L"\xe7\x99\xbd\xe7\x8c\xab \xe9\xbb\x91\xe7\x8b\x97 \xe7\x94\xb2\xe8\x99\xab", 9);
    src = L"\xe7\x94\xb2\xe8\x99\xab,\xe9\xbb\x91\xe7\x8b\x97";
    len = wcscspn(dest, src);
    dest[len] = 0;
    setlocale(0, "en_US.utf8");
    wprintf(L"The length of maximum initial segment is %td.\nThe segment is \"%ls\".\n", len, dest);
}

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &_OptionsStorage;
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


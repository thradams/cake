/* Cake 0.13.29 x86_msvc */

struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

static void memcpy2(void * dest, const void * src, unsigned int n);
__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;

unsigned short *__cdecl wcscat(unsigned short * _Destination, unsigned short * _Source);
char *__cdecl setlocale(int _Category, char * _Locale);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    unsigned short str[50];

    memcpy2(str, L"\xd0\x97\xd0\xb5\xd0\xbc\xd0\xbb\xd1\x8f, \xd0\xbf\xd1\x80\xd0\xbe\xd1\x89\xd0\xb0\xd0\xb9.", 50);
    wcscat(str, L" ");
    wcscat(str, L"\xd0\x92 \xd0\xb4\xd0\xbe\xd0\xb1\xd1\x80\xd1\x8b\xd0\xb9 \xd0\xbf\xd1\x83\xd1\x82\xd1\x8c.");
    setlocale(0, "en_US.utf8");
    printf("%ls", str);
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

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

static void memcpy2(void * dest, const void * src, unsigned int n)
{
    char *csrc;
    char *cdest;
    unsigned int i; 

    csrc = (char *)src;
    cdest = (char *)dest;
    for (i = 0; i < n; i++)
    {
       cdest[i] = csrc[i]; 
    }
}


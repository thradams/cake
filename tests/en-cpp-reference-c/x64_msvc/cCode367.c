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
__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;

int __cdecl mbtowc(unsigned short * _DstCh, char * _SrcCh, unsigned long long _SrcSizeInBytes);
unsigned long long __cdecl strlen(char * _Str);
static int __cdecl wprintf(unsigned short * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfwprintf(unsigned long long _Options, struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void print_mb(char * ptr)
{
    char * end;
    int ret;

    mbtowc(0, 0, 0);
    end = ptr + strlen(ptr);
    ret = 0;
    {
        unsigned short wc;

        for (; (ret = mbtowc(&wc, ptr, end - ptr)) > 0; ptr += ret)
        {
            wprintf(L"%lc", wc);
        }
    }
    wprintf(L"\n");
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

char *__cdecl setlocale(int _Category, char * _Locale);

int main(void)
{
    setlocale(0, "en_US.utf8");
    print_mb("z\u00df\u6c34\U0001F34C");
}

/* Cake x64_msvc */

struct _iobuf {
    void * _Placeholder;
};

struct _Mbstatet {
    unsigned long _Wchar;
    unsigned short _Byte;
    unsigned short _State;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};
__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;

char *__cdecl setlocale(int _Category, char * _Locale);
unsigned long long __cdecl mbrlen(char * _Ch, unsigned long long _SizeInBytes, struct _Mbstatet * _State);
static struct _Mbstatet mb;
static int __cdecl mbsinit(struct _Mbstatet * _P);
static int __cdecl printf(char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
unsigned long long __cdecl strlen(char * _Str);

int main(void)
{
    char * str;

    setlocale(0, "en_US.utf8");
    str = "\xe6\xb0\xb4";
    (void)mbrlen(&str[0], 1, &mb);
    if (!mbsinit(&mb))
    {
        printf("After processing the first 1 byte of %s,\nthe conversion state is not initial\n\n", str);
    }
    (void)mbrlen(&str[1], strlen(str), &mb);
    if (mbsinit(&mb))
    {
        printf("After processing the remaining 2 bytes of %s,\nthe conversion state is initial conversion state\n", str);
    }
}

static int __cdecl mbsinit(struct _Mbstatet * _P)
{
    return _P == 0 || _P->_Wchar == 0;
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

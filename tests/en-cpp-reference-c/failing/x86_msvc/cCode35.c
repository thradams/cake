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
__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;

char *__cdecl tmpnam(char * _Buffer);
struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);
int __cdecl fputs(char * _Buffer, struct _iobuf * _Stream);
int __cdecl fclose(struct _iobuf * _Stream);
char *__cdecl setlocale(int _Category, char * _Locale);
unsigned short __cdecl fgetwc(struct _iobuf * _Stream);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl feof(struct _iobuf * _Stream);
int __cdecl puts(char * _Buffer);
int __cdecl ferror(struct _iobuf * _Stream);

int main(void)
{
    char * fname;
    struct _iobuf * f;
    unsigned short ch;

    fname = tmpnam(0);
    f = fopen(fname, "wb");
    fputs("\xff\xff\n", f);
    fclose(f);
    setlocale(0, "en_US.utf8");
    f = fopen(fname, "rb");
    while ((ch = fgetwc(f)) != 65535)
    printf("%#x ", ch);
    if (feof(f))
    {
        puts("EOF indicator set");
    }
    if (ferror(f))
    {
        puts("Error indicator set");
    }
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

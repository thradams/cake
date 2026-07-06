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

static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    int c1;
    int c2;
    int c3;
    unsigned short uc1;
    unsigned short uc2;
    unsigned short uc3;
    unsigned short uc4;
    unsigned int Uc1;
    unsigned int Uc2;
    unsigned int Uc3;
    unsigned int Uc4;
    unsigned short wc1;
    unsigned short wc2;
    unsigned short wc3;
    unsigned short wc4;

    printf("constant value     \n");
    printf("-------- ----------\n");
    c1 = 97;
    printf("'a':\t %#010x\n", c1);
    c2 = 76;
    printf("'\xf0\x9f\x8d\x8c':\t %#010x\n\n", c2);
    c3 = 24930;
    printf("'ab':\t %#010x\n\n", c3);
    uc1 = 97;
    printf("'a':\t %#010x\n", (int)uc1);
    uc2 = 162;
    printf("'\xc2\xa2':\t %#010x\n", (int)uc2);
    uc3 = 29483;
    printf("'\xe7\x8c\xab':\t %#010x\n", (int)uc3);
    uc4 = 62284;
    printf("'\xf0\x9f\x8d\x8c':\t %#010x\n\n", (int)uc4);
    Uc1 = 97;
    printf("'a':\t %#010x\n", (int)Uc1);
    Uc2 = 162;
    printf("'\xc2\xa2':\t %#010x\n", (int)Uc2);
    Uc3 = 29483;
    printf("'\xe7\x8c\xab':\t %#010x\n", (int)Uc3);
    Uc4 = 62284;
    printf("'\xf0\x9f\x8d\x8c':\t %#010x\n\n", (int)Uc4);
    wc1 = 97;
    printf("'a':\t %#010x\n", (int)wc1);
    wc2 = 162;
    printf("'\xc2\xa2':\t %#010x\n", (int)wc2);
    wc3 = 29483;
    printf("'\xe7\x8c\xab':\t %#010x\n", (int)wc3);
    wc4 = 62284;
    printf("'\xf0\x9f\x8d\x8c':\t %#010x\n\n", (int)wc4);
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

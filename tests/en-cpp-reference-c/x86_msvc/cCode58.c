/* Cake 0.12.26 x86_msvc */
struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


int __cdecl putchar(int _Character);
int __cdecl ferror(struct _iobuf * _Stream);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
static int __cdecl fprintf(struct _iobuf * _Stream, char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
void __cdecl perror(char * _ErrorMessage);
void __cdecl exit(int _Code);
static int __cdecl printf(char * _Format, ...);

int main(void)
{
    int ret_code;
    int r;

    ret_code = 0;
    {
        char c;

        c = 97;
        for (; (ret_code != -1) && (c != 122); c++)
        ret_code = putchar(c);
    }
    if (ret_code == -1 && ferror((__acrt_iob_func(1))))
    {
        fprintf((__acrt_iob_func(2)), "putchar() failed in file %s at line # %d\n", "c:/users/thiag/source/repos/cake/tests/en-cpp-reference-c/ccode58.c", 9);
        perror("putchar()");
        exit(1);
    }
    putchar(10);
    r = 4208;
    printf("\n0x%x\n", r);
    r = putchar(r);
    printf("\n0x%x\n", r);
}



static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__c0__OptionsStorage;
}

static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl fprintf(struct _iobuf * _Stream, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vfprintf_l(_Stream, _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
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

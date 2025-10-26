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


static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int *__cdecl _errno(void);
long __cdecl strtol(char * _String, char ** _EndPtr, int _Radix);
int __cdecl putchar(int _Character);

int main(void)
{
    char * p;

    p = "10 200000000000000000000000000000 30 -40 junk";
    printf("Parsing '%s':\n", p);
    for (; ; )
    {
        char * end;
        long i;
        unsigned char  range_error;

        (*_errno()) = 0;
        i = strtol(p, &end, 10);
        if (p == end)
        {
            break;
        }
        range_error = (*_errno()) == 34;
        printf("Extracted '%.*s', strtol returned %ld.", (int)(end - p), p, i);
        p = end;
        if (range_error)
        {
            printf("\n --> Range error occurred.");
        }
        putchar(10);
    }
    printf("Unextracted leftover: '%s'\n\n", p);
    printf("\"1010\" in binary  --> %ld\n", strtol("1010", 0, 2));
    printf("\"12\"   in octal   --> %ld\n", strtol("12", 0, 8));
    printf("\"A\"    in hex     --> %ld\n", strtol("A", 0, 16));
    printf("\"junk\" in base-36 --> %ld\n", strtol("junk", 0, 36));
    printf("\"012\"  in auto-detected base --> %ld\n", strtol("012", 0, 0));
    printf("\"0xA\"  in auto-detected base --> %ld\n", strtol("0xA", 0, 0));
    printf("\"junk\" in auto-detected base --> %ld\n", strtol("junk", 0, 0));
}



static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__c0__OptionsStorage;
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

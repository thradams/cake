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


char *__cdecl setlocale(int _Category, char * _Locale);
unsigned int __cdecl strxfrm(char * _Destination, char * _Source, unsigned int _MaxCount);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl strcmp(char * _Str1, char * _Str2);

int main(void)
{
    char * in1;
    char out1[];
    char * in2;
    char out2[];

    setlocale(1, "cs_CZ.iso88592");
    in1 = "hrnec";
    strxfrm(out1, in1, 0);
    in2 = "chrt";
    strxfrm(out2, in2, 0);
    printf("In the Czech locale: ");
    if (strcmp(out1, out2) < 0)
    {
        printf("%s before %s\n", in1, in2);
    }
    else
    {
        printf("%s before %s\n", in2, in1);
    }
    printf("In lexicographical comparison: ");
    if (strcmp(in1, in2) < 0)
    {
        printf("%s before %s\n", in1, in2);
    }
    else
    {
        printf("%s before %s\n", in2, in1);
    }
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

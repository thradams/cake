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


int __cdecl strncmp(char * _Str1, char * _Str2, unsigned int _MaxCount);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void demo(char * lhs, char * rhs, int sz)
{
    int rc;

    rc = strncmp(lhs, rhs, sz);
    if (rc < 0)
    {
        printf("First %d chars of [%s] precede [%s]\n", sz, lhs, rhs);
    }
    else
    {
        if (rc > 0)
        {
            printf("First %d chars of [%s] follow [%s]\n", sz, lhs, rhs);
        }
        else
        {
            printf("First %d chars of [%s] equal [%s]\n", sz, lhs, rhs);
        }
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
int main(void)
{
    char * string;

    string = "Hello World!";
    demo(string, "Hello!", 5);
    demo(string, "Hello", 10);
    demo(string, "Hello there", 10);
    demo("Hello, everybody!" + 12, "Hello, somebody!" + 11, 5);
}



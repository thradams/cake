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
unsigned long __cdecl strtoul(char * _String, char ** _EndPtr, int _Radix);
int *__cdecl _errno(void);

int main(void)
{
    char * p;
    char * end;

    p = "10 200000000000000000000000000000 30 -40 - 42";
    printf("Parsing '%s':\n", p);
    end = 0;
    {
        unsigned long i;

        i = strtoul(p, &end, 10);
        for (; p != end; i = strtoul(p, &end, 10))
        {
            printf("'%.*s' -> ", (int)(end - p), p);
            p = end;
            if ((*_errno()) == 34)
            {
                (*_errno()) = 0;
                printf("range error, got ");
            }
            printf("%lu\n", i);
        }
    }
    printf("After the loop p points to '%s'\n", p);
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

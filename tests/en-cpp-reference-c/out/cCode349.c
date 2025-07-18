struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};




int __cdecl __stdio_common_vfprintf(unsigned __int64 _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

static unsigned __int64 __ck__OptionsStorage1;
inline unsigned __int64 *__cdecl __local_stdio_printf_options(void)
{
    return &__ck__OptionsStorage1;
}
inline int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
inline int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}
unsigned long __cdecl strtoul(char * _String, char ** _EndPtr, int _Radix);
int *__cdecl _errno(void);

int main(void)
{
    char * p;
    char * end;

    p = "10 200000000000000000000000000000 30 -40 - 42";
    printf("Parsing '%s':\n", p);
    end = 0U;
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


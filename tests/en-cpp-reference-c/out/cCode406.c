struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


int _Avx2WmemEnabledWeakValue = 0;


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
long __cdecl wcstol(unsigned short * _String, unsigned short ** _EndPtr, int _Radix);
int *__cdecl _errno(void);

int main(void)
{
    unsigned short * p;
    unsigned short * end;

    p = L"10 200000000000000000000000000000 30 -40";
    printf("Parsing L'%ls':\n", p);
    {
        long i;
        i = wcstol(p, &end, 10);
        for (; p != end; i = wcstol(p, &end, 10))
        {
            printf("'%.*ls' -> ", (int)(end - p), p);
            p = end;
            if ((*_errno()) == 34)
            {
                printf("range error, got ");
                (*_errno()) = 0;
            }
            printf("%ld\n", i);
        }
    }
}


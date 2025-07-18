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
double __cdecl strtod(char * _String, char ** _EndPtr);
int *__cdecl _errno(void);

int main(void)
{
    char * p;
    char * end;

    p = "111.11 -2.22 Nan nan(2) inF 0X1.BC70A3D70A3D7P+6  1.18973e+4932zzz";
    printf("Parsing '%s':\n", p);
    {
        double f;
        f = strtod(p, &end);
        for (; p != end; f = strtod(p, &end))
        {
            printf("'%.*s' -> ", (int)(end - p), p);
            p = end;
            if ((*_errno()) == 34)
            {
                printf("range error, got ");
                (*_errno()) = 0;
            }
            printf("%f\n", f);
        }
    }
    printf("\"  -0.0000000123junk\"  -->  %g\n", strtod("  -0.0000000123junk", 0U));
    printf("\"junk\"                 -->  %g\n", strtod("junk", 0U));
}


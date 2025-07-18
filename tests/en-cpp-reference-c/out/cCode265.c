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
double __cdecl pow(double _X, double _Y);
int *__cdecl _errno(void);
int __cdecl feclearexcept(int _Flags);
void __cdecl perror(char * _ErrorMessage);
int __cdecl fetestexcept(int _Flags);
int __cdecl puts(char * _Buffer);

int main(void)
{
    printf("pow(2, 10) = %f\n", pow(2, 10));
    printf("pow(2, 0.5) = %f\n", pow(2, 0.500000));
    printf("pow(-2, -3) = %f\n", pow(-2, -3));
    printf("pow(-1, NAN) = %f\n", pow(-1, -0.000000f));
    printf("pow(+1, NAN) = %f\n", pow(1, -0.000000f));
    printf("pow(INFINITY, 2) = %f\n", pow(1.000000f, 2));
    printf("pow(INFINITY, -1) = %f\n", pow(1.000000f, -1));
    (*_errno()) = 0;
    feclearexcept(31);
    printf("pow(-1, 1/3) = %f\n", pow(-1, 0.333333));
    if ((*_errno()) == 33)
    {
        perror("    errno == EDOM");
    }
    if (fetestexcept(16))
    {
        puts("    FE_INVALID raised");
    }
    feclearexcept(31);
    printf("pow(-0, -3) = %f\n", pow(-0.000000, -3));
    if (fetestexcept(8))
    {
        puts("    FE_DIVBYZERO raised");
    }
}


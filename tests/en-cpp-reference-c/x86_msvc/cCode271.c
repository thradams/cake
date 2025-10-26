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
double __cdecl scalbn(double _X, int _Y);
double __cdecl nextafter(double _X, double _Y);
int *__cdecl _errno(void);
int __cdecl feclearexcept(int _Flags);
void __cdecl perror(char * _ErrorMessage);
int __cdecl fetestexcept(int _Flags);
int __cdecl puts(char * _Buffer);

int main(void)
{
    printf("scalbn(7, -4) = %f\n", scalbn(7, -4));
    printf("scalbn(1, -1074) = %g (minimum positive subnormal double)\n", scalbn(1, -1074));
    printf("scalbn(nextafter(1,0), 1024) = %g (largest finite double)\n", scalbn(nextafter(1, 0), 1024));
    printf("scalbn(-0, 10) = %f\n", scalbn(-0.0, 10));
    printf("scalbn(-Inf, -1) = %f\n", scalbn(-inff, -1));
    (*_errno()) = 0;
    feclearexcept(31);
    printf("scalbn(1, 1024) = %f\n", scalbn(1, 1024));
    if ((*_errno()) == 34)
    {
        perror("    errno == ERANGE");
    }
    if (fetestexcept(4))
    {
        puts("    FE_OVERFLOW raised");
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

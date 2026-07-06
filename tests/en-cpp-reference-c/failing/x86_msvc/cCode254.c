/* Cake x86_msvc */
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
double __cdecl lgamma(double _X);
double __cdecl log(double _X);
double __cdecl acos(double _X);
double __cdecl sqrt(double _X);
int *__cdecl _errno(void);
int __cdecl feclearexcept(int _Flags);
void __cdecl perror(char * _ErrorMessage);
int __cdecl fetestexcept(int _Flags);
int __cdecl puts(char * _Buffer);

int main(void)
{
    double pi;

    printf("lgamma(10) = %f, log(9!) = %f\n", lgamma(10), log(362880));
    pi = acos(-1);
    printf("lgamma(0.5) = %f, log(sqrt(pi)) = %f\n", log(sqrt(pi)), lgamma(0.5));
    printf("lgamma(1) = %f\n", lgamma(1));
    printf("lgamma(+Inf) = %f\n", lgamma(.7976931348623157E+308f));
    (*_errno()) = 0;
    feclearexcept(31);
    printf("lgamma(0) = %f\n", lgamma(0));
    if ((*_errno()) == 34)
    {
        perror("    errno == ERANGE");
    }
    if (fetestexcept(8))
    {
        puts("    FE_DIVBYZERO raised");
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

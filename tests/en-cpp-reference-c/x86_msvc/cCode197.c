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
int __cdecl fetestexcept(int _Flags);
int __cdecl feclearexcept(int _Flags);

void show_fe_exceptions(void)
{
    printf("exceptions raised:");
    if (fetestexcept(8))
    {
        printf(" FE_DIVBYZERO");
    }
    if (fetestexcept(1))
    {
        printf(" FE_INEXACT");
    }
    if (fetestexcept(16))
    {
        printf(" FE_INVALID");
    }
    if (fetestexcept(4))
    {
        printf(" FE_OVERFLOW");
    }
    if (fetestexcept(2))
    {
        printf(" FE_UNDERFLOW");
    }
    feclearexcept(31);
    printf("\n");
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
double __cdecl sqrt(double _X);
double __cdecl nextafter(double _X, double _Y);
double __cdecl pow(double _X, double _Y);

int main(void)
{
    printf("MATH_ERREXCEPT is %s\n", 2 ? "set" : "not set");
    printf("0.0/0.0 = %f\n", -nan(ind).0);
    show_fe_exceptions();
    printf("1.0/0.0 = %f\n", -nan(ind).0);
    show_fe_exceptions();
    printf("1.0/10.0 = %f\n", 0.0);
    show_fe_exceptions();
    printf("sqrt(-1) = %f\n", sqrt(-1));
    show_fe_exceptions();
    printf("DBL_MAX*2.0 = %f\n", -nan.0);
    show_fe_exceptions();
    printf("nextafter(DBL_MIN/pow(2.0,52),0.0) = %.1f\n", nextafter(2.2250738585072014e-308 / pow(2.0, 52), 0.0));
    show_fe_exceptions();
}



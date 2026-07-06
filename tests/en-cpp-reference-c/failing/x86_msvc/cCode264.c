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


float __cdecl nextafterf(float _X, float _Y);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
double __cdecl nextafter(double _X, double _Y);
long double __cdecl nextafterl(long double _X, long double _Y);
float __cdecl nexttowardf(float _X, long double _Y);
int __cdecl feclearexcept(int _Flags);
int __cdecl fetestexcept(int _Flags);
int __cdecl puts(char * _Buffer);

int main(void)
{
    float from1;
    float to1;
    float from2;
    float to2;
    double from3;
    double to3;
    long double dir;
    float x;
    float from5;
    float to5;

    from1 = 0;
    to1 = nextafterf(from1, 1);
    printf("The next representable float after %.2f is %.20g (%a)\n", from1, to1, to1);
    from2 = 1;
    to2 = nextafterf(from2, 2);
    printf("The next representable float after %.2f is %.20f (%a)\n", from2, to2, to2);
    from3 = nextafter(0.10000000000000001, 0);
    to3 = 0.10000000000000001;
    printf("The number 0.1 lies between two valid doubles:\n    %.56f (%a)\nand %.55f  (%a)\n", from3, from3, to3, to3);
    dir = nextafterl(from1, 1);
    x = nextafterf(from1, dir);
    printf("Using nextafter, next float after %.2f (%a) is %.20g (%a)\n", from1, from1, x, x);
    x = nexttowardf(from1, dir);
    printf("Using nexttoward, next float after %.2f (%a) is %.20g (%a)\n", from1, from1, x, x);
    {
        double from4;
        double to4;

        feclearexcept(31);
        from4 = 1.7976931348623157e+308;
        to4 = nextafter(from4, .7976931348623157E+308f);
        printf("The next representable double after %.2g (%a) is %.23f (%a)\n", from4, from4, to4, to4);
        if (fetestexcept(4))
        {
            puts("   raised FE_OVERFLOW");
        }
        if (fetestexcept(1))
        {
            puts("   raised FE_INEXACT");
        }
    }
    from5 = 0.0;
    to5 = nextafter(from5, -0.0);
    printf("nextafter(+0.0, -0.0) gives %.2g (%a)\n", to5, to5);
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

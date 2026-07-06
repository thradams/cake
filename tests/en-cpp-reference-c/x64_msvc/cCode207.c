/* Cake x64_msvc */

struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

short __cdecl _fdclass(float _X);
short __cdecl _dclass(double _X);
short __cdecl _ldclass(long double _X);

char *show_classification(double x)
{
    /*switch*/
    {
        int __a = (0 ? _fdclass((float)(x)) : 1 ? _dclass((double)(x)) : _ldclass((long double)(x)));
        if (__a == 1) goto __L1; /*case 1*/
        if (__a == 2) goto __L2; /*case 2*/
        if (__a == -1) goto __L3; /*case -1*/
        if (__a == -2) goto __L4; /*case -2*/
        if (__a == 0) goto __L5; /*case 0*/
        goto __L6; /* default */

        {
            __L1: /*case 1*/ 
            return "Inf";
            __L2: /*case 2*/ 
            return "NaN";
            __L3: /*case -1*/ 
            return "normal";
            __L4: /*case -2*/ 
            return "subnormal";
            __L5: /*case 0*/ 
            return "zero";
            __L6: /* default */ 
            return "unknown";
        }
        __L0:;
    }
}

static int __cdecl printf(char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    printf("1.0/0.0 is %s\n", show_classification(.7976931348623157E+308));
    printf("0.0/0.0 is %s\n", show_classification(-nan(ind).0));
    printf("DBL_MIN/2 is %s\n", show_classification(1.1125369292536007e-308));
    printf("-0.0 is %s\n", show_classification(-0.0));
    printf(" 1.0 is %s\n", show_classification(1.0));
}

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &_OptionsStorage;
}

static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(__va_start(&_ArgList, _Format)));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

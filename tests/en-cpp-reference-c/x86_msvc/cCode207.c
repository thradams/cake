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


short __cdecl _fdclass(float _X);
short __cdecl _dclass(double _X);
short __cdecl _ldclass(long double _X);

char *show_classification(double x)
{
    /*switch*/
    {
        int __v0 = (0 ? _fdclass((float)(x)) : 1 ? _dclass((double)(x)) : _ldclass((long double)(x)));
        if (__v0 == 1) goto __L1; /*case 1*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == -1) goto __L3; /*case -1*/
        if (__v0 == -2) goto __L4; /*case -2*/
        if (__v0 == 0) goto __L5; /*case 0*/
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
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    printf("1.0/0.0 is %s\n", show_classification(-nan(ind).0));
    printf("0.0/0.0 is %s\n", show_classification(-nan(ind).0));
    printf("DBL_MIN/2 is %s\n", show_classification(0.0));
    printf("-0.0 is %s\n", show_classification(-0.0));
    printf(" 1.0 is %s\n", show_classification(1.0));
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

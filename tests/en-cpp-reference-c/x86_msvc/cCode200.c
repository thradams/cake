/* Cake 0.12.26 x86_msvc */
struct fenv_t {
    unsigned long _Fe_ctl;
    unsigned long _Fe_stat;
};

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

void show_fe_exceptions(void)
{
    printf("current exceptions raised: ");
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
    if (fetestexcept(31) == 0)
    {
        printf(" none");
    }
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
int __cdecl fegetround(void);

void show_fe_rounding_method(void)
{
    printf("current rounding method:    ");
    /*switch*/
    {
        int __v0 = fegetround();
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 256) goto __L2; /*case 256*/
        if (__v0 == 512) goto __L3; /*case 512*/
        if (__v0 == 768) goto __L4; /*case 768*/
        goto __L5; /* default */

        {
            __L1: /*case 0*/ 
            printf("FE_TONEAREST");
            goto __L0; /* break */

            __L2: /*case 256*/ 
            printf("FE_DOWNWARD");
            goto __L0; /* break */

            __L3: /*case 512*/ 
            printf("FE_UPWARD");
            goto __L0; /* break */

            __L4: /*case 768*/ 
            printf("FE_TOWARDZERO");
            goto __L0; /* break */

            __L5: /* default */ 
            printf("unknown");
        }
        __L0:;
    }
    ;
    printf("\n");
}


void show_fe_environment(void)
{
    show_fe_exceptions();
    show_fe_rounding_method();
}


double __cdecl rint(double _X);
int __cdecl fegetenv(struct fenv_t * _Env);
int __cdecl feclearexcept(int _Flags);
int __cdecl fesetround(int _Round);
int __cdecl fesetenv(struct fenv_t * _Env);

int main(void)
{
    struct fenv_t  curr_env;
    int rtn;

    show_fe_environment();
    printf("\n");
    printf("+11.5 -> %+4.1f\n", rint(11.5));
    printf("+12.5 -> %+4.1f\n", rint(12.5));
    show_fe_environment();
    printf("\n");
    rtn = fegetenv(&curr_env);
    feclearexcept(31);
    fesetround(256);
    printf("1.0/0.0 = %f\n", -nan(ind).0);
    printf("+11.5 -> %+4.1f\n", rint(11.5));
    printf("+12.5 -> %+4.1f\n", rint(12.5));
    show_fe_environment();
    printf("\n");
    rtn = fesetenv(&curr_env);
    show_fe_environment();
    return 0;
}



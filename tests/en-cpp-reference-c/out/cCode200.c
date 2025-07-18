struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};

struct fenv_t {
    unsigned long _Fe_ctl;
    unsigned long _Fe_stat;
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

int __cdecl fegetround(void);

void show_fe_rounding_method(void)
{
    printf("current rounding method:    ");
    /*switch*/
    {
        register int _R0 = fegetround();
        if (_R0 == 0) goto _CKL1; /*case 0*/
        if (_R0 == 256) goto _CKL2; /*case 256*/
        if (_R0 == 512) goto _CKL3; /*case 512*/
        if (_R0 == 768) goto _CKL4; /*case 768*/
        goto /*default*/ _CKL5;

        {
            /*case 0*/ _CKL1:
            printf("FE_TONEAREST");
            /*break*/ goto _CKL0;

            /*case 256*/ _CKL2:
            printf("FE_DOWNWARD");
            /*break*/ goto _CKL0;

            /*case 512*/ _CKL3:
            printf("FE_UPWARD");
            /*break*/ goto _CKL0;

            /*case 768*/ _CKL4:
            printf("FE_TOWARDZERO");
            /*break*/ goto _CKL0;

            /*default*/ _CKL5:
            printf("unknown");
        }
        _CKL0:;
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
    printf("+11.5 -> %+4.1f\n", rint(11.500000));
    printf("+12.5 -> %+4.1f\n", rint(12.500000));
    show_fe_environment();
    printf("\n");
    rtn = fegetenv(&curr_env);
    feclearexcept(31);
    fesetround(256);
    printf("1.0/0.0 = %f\n", 1.000000 / 0.000000);
    printf("+11.5 -> %+4.1f\n", rint(11.500000));
    printf("+12.5 -> %+4.1f\n", rint(12.500000));
    show_fe_environment();
    printf("\n");
    rtn = fesetenv(&curr_env);
    show_fe_environment();
    return 0;
}


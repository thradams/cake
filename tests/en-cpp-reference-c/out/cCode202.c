struct _iobuf {
    void * _Placeholder;
};

struct _struct_tag_0 {
    int _Except_Val;
    double _Num;
    double _Denom;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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

int __cdecl feholdexcept(struct fenv_t * _Env);
int __cdecl feclearexcept(int _Flags);

int __cdecl fesetenv(struct fenv_t * _Env);

static struct _struct_tag_0 __ck__Table2[5] = {16, 0.000000, 0.000000, 8, 1.000000, 0.000000, 4, 1, 1, 2, 1, 1, 1, 2.000000, 3.000000};

inline int __cdecl feraiseexcept(int _Except)
{
    double _Ans;
    unsigned int _Index;

    _Ans = 0.000000;
    (void)_Ans;
    if ((_Except &= 31) == 0)
    {
        return 0;
    }
    for (_Index = 0; _Index < 5U; ++_Index)
    {
        if ((_Except & __ck__Table2[_Index]._Except_Val) != 0)
        {
            _Ans = __ck__Table2[_Index]._Num / __ck__Table2[_Index]._Denom;
        }
    }
    return 0;
}
inline int __cdecl feupdateenv(struct fenv_t * _Penv)
{
    int _Except;

    _Except = fetestexcept(31);
    if (fesetenv(_Penv) != 0 || feraiseexcept(_Except) != 0)
    {
        return 1;
    }
    return 0;
}

double x2(double x)
{
    struct fenv_t  curr_excepts;

    feholdexcept(&curr_excepts);
    printf("In x2():  x = %f\n", x = x * 2.000000);
    show_fe_exceptions();
    feclearexcept(1);
    feupdateenv(&curr_excepts);
    return x;
}

int main(void)
{
    feclearexcept(31);
    feraiseexcept(16);
    show_fe_exceptions();
    printf("x2(DBL_MAX) = %f\n", x2(179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000));
    show_fe_exceptions();
    return 0;
}


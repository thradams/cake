struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _struct_tag_0 {
    int _Except_Val;
    double _Num;
    double _Denom;
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
int __cdecl fetestexcept(int _Flags);
int __cdecl feclearexcept(int _Flags);

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
    feclearexcept(31);
    printf("\n");
}


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

double some_computation(void)
{
    int r;

    r = feraiseexcept(5);
    printf("feraiseexcept() %s\n", (r ? "fails" : "succeeds"));
    return 0.000000;
}

int main(void)
{
    some_computation();
    show_fe_exceptions();
    return 0;
}


/* Cake x64_msvc */

struct __tag0 {
    int _Except_Val;
    double _Num;
    double _Denom;
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
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
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

static int __cdecl feraiseexcept(int _Except);
static struct __tag0 _Table[5] = {16,0.0,0.0,8,1.0,0.0,4,1.0000000000000001e+300,1e-300,2,1e-300,1.0000000000000001e+300,1,2.0,3.0};

int __cdecl fegetexceptflag(unsigned long * _Except, int _TestFlags);
int __cdecl feclearexcept(int _Flags);
int __cdecl fesetexceptflag(unsigned long * _Except, int _SetFlags);

int main(void)
{
    unsigned long excepts;

    feraiseexcept(16);
    show_fe_exceptions();
    fegetexceptflag(&excepts, 31);
    feclearexcept(31);
    feraiseexcept(5);
    show_fe_exceptions();
    fesetexceptflag(&excepts, 31);
    show_fe_exceptions();
    return 0;
}

static int __cdecl feraiseexcept(int _Except)
{
    double _Ans;
    unsigned long long _Index;

    _Ans = 0.0;
    (void)_Ans;
    if ((_Except &= 31) == 0)
    {
        return 0;
    }
    for (_Index = 0; _Index < 5ULL; ++_Index)
    {
        if ((_Except & _Table[_Index]._Except_Val) != 0)
        {
            _Ans = _Table[_Index]._Num / _Table[_Index]._Denom;
        }
    }
    return 0;
}

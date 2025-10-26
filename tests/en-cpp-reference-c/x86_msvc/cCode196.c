/* Cake 0.12.26 x86_msvc */
struct __tag0 {
    int _Except_Val;
    double _Num;
    double _Denom;
};

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


int __cdecl fesetround(int _Round);
static int __cdecl feraiseexcept(int _Except);
static struct __tag0 __c1__Table[5] = {16,0.0,0.0,8,1.0,0.0,4,1.0000000000000001e+300,1e-300.0,2,1e-300.0,1.0000000000000001e+300,1,2.0,3.0};

int __cdecl fesetenv(struct fenv_t * _Env);
extern struct fenv_t  _Fenv1 = {1061097535,0};


int main()
{
    printf("On startup:\n");
    show_fe_environment();
    fesetround(256);
    feraiseexcept(16);
    printf("\nBefore restoration:\n");
    show_fe_environment();
    fesetenv((&_Fenv1));
    printf("\nAfter restoring default environment:\n");
    show_fe_environment();
}



static int __cdecl feraiseexcept(int _Except)
{
    double _Ans;
    unsigned int _Index;

    _Ans = 0.0;
    (void)_Ans;
    if ((_Except &= 31) == 0)
    {
        return 0;
    }
    for (_Index = 0; _Index < 5; ++_Index)
    {
        if ((_Except & __c1__Table[_Index]._Except_Val) != 0)
        {
            _Ans = __c1__Table[_Index]._Num / __c1__Table[_Index]._Denom;
        }
    }
    return 0;
}

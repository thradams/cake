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

static int __cdecl printf(char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl fegetround(void);

void show_fe_current_rounding_direction(void)
{
    printf("current rounding direction:  ");
    /*switch*/
    {
        int __a = fegetround();
        if (__a == 0) goto __L1; /*case 0*/
        if (__a == 256) goto __L2; /*case 256*/
        if (__a == 512) goto __L3; /*case 512*/
        if (__a == 768) goto __L4; /*case 768*/
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

double __cdecl rint(double _X);
int __cdecl fesetround(int _Round);

int main(void)
{
    int curr_direction;

    show_fe_current_rounding_direction();
    printf("+11.5 -> %+4.1f\n", rint(11.5));
    printf("+12.5 -> %+4.1f\n", rint(12.5));
    curr_direction = fegetround();
    fesetround(256);
    show_fe_current_rounding_direction();
    printf("+11.5 -> %+4.1f\n", rint(11.5));
    printf("+12.5 -> %+4.1f\n", rint(12.5));
    fesetround(curr_direction);
    show_fe_current_rounding_direction();
    return 0;
}

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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
int __cdecl fegetround(void);

void show_fe_current_rounding_direction(void)
{
    printf("current rounding direction:  ");
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

double __cdecl rint(double _X);
int __cdecl fesetround(int _Round);

int main(void)
{
    int curr_direction;

    show_fe_current_rounding_direction();
    printf("+11.5 -> %+4.1f\n", rint(11.500000));
    printf("+12.5 -> %+4.1f\n", rint(12.500000));
    curr_direction = fegetround();
    fesetround(256);
    show_fe_current_rounding_direction();
    printf("+11.5 -> %+4.1f\n", rint(11.500000));
    printf("+12.5 -> %+4.1f\n", rint(12.500000));
    fesetround(curr_direction);
    show_fe_current_rounding_direction();
    return 0;
}


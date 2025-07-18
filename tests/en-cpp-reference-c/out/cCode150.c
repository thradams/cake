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

void func(int x)
{
    printf("func(%d): ", x);
    /*switch*/
    {
        register int _R0 = x;
        if (_R0 == 1) goto _CKL1; /*case 1*/
        if (_R0 == 2) goto _CKL2; /*case 2*/
        if (_R0 == 3) goto _CKL3; /*case 3*/
        if (_R0 == 4) goto _CKL4; /*case 4*/
        if (_R0 == 5) goto _CKL5; /*case 5*/
        if (_R0 == 6) goto _CKL6; /*case 6*/
        goto /*default*/ _CKL7;

        {
            /*case 1*/ _CKL1:
            printf("case 1, ");
            /*case 2*/ _CKL2:
            printf("case 2, ");
            /*case 3*/ _CKL3:
            printf("case 3.\n");
            /*break*/ goto _CKL0;

            /*case 4*/ _CKL4:
            printf("case 4, ");
            /*case 5*/ _CKL5:
            /*case 6*/ _CKL6:
            printf("case 5 or case 6, ");
            /*default*/ _CKL7:
            printf("default.\n");
        }
        _CKL0:;
    }
}

int main(void)
{
    {
        int i;
        i = 1;
        for (; i < 9; ++i)
        func(i);
    }
}


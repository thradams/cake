struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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

int main(void)
{
    int i;

    i = 2;
    /*switch*/
    {
        register int _R0 = i;
        if (_R0 == 1) goto _CKL1; /*case 1*/
        if (_R0 == 2) goto _CKL2; /*case 2*/
        if (_R0 == 3) goto _CKL3; /*case 3*/
        if (_R0 == 4) goto _CKL4; /*case 4*/
        if (_R0 == 5) goto _CKL5; /*case 5*/
        if (_R0 == 6) goto _CKL6; /*case 6*/
        goto _CKL0;

        {
            /*case 1*/ _CKL1:
            printf("1");
            /*case 2*/ _CKL2:
            printf("2");
            /*case 3*/ _CKL3:
            printf("3");
            /*case 4*/ _CKL4:
            /*case 5*/ _CKL5:
            printf("45");
            /*break*/ goto _CKL0;

            /*case 6*/ _CKL6:
            printf("6");
        }
        _CKL0:;
    }
    printf("\n");
    {
        int j;
        j = 0;
        for (; j < 2; j++)
        {
            int k;
            k = 0;
            for (; k < 5; k++)
            printf("%d%d ", j, k);
        }
    }
    printf("\n");
    {
        int j;
        j = 0;
        for (; j < 2; j++)
        {
            {
                int k;
                k = 0;
                for (; k < 5; k++)
                {
                    if (k == 2)
                    {
                        break;
                    }
                    printf("%d%d ", j, k);
                }
            }
        }
    }
}


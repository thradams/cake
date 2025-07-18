struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


short __cdecl _fdclass(float _X);
short __cdecl _dclass(double _X);
short __cdecl _ldclass(long double _X);

char *show_classification(double x)
{
    /*switch*/
    {
        register int _R0 = ((0 ? _fdclass((float)(x)) : 1 ? _dclass((double)(x)) : _ldclass((long double)(x))));
        if (_R0 == 1) goto _CKL1; /*case 1*/
        if (_R0 == 2) goto _CKL2; /*case 2*/
        if (_R0 == -1) goto _CKL3; /*case -1*/
        if (_R0 == -2) goto _CKL4; /*case -2*/
        if (_R0 == 0) goto _CKL5; /*case 0*/
        goto /*default*/ _CKL6;

        {
            /*case 1*/ _CKL1:
            return "Inf";
            /*case 2*/ _CKL2:
            return "NaN";
            /*case -1*/ _CKL3:
            return "normal";
            /*case -2*/ _CKL4:
            return "subnormal";
            /*case 0*/ _CKL5:
            return "zero";
            /*default*/ _CKL6:
            return "unknown";
        }
        _CKL0:;
    }
}



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
    printf("1.0/0.0 is %s\n", show_classification(1 / 0.000000));
    printf("0.0/0.0 is %s\n", show_classification(0.000000 / 0.000000));
    printf("DBL_MIN/2 is %s\n", show_classification(0.000000));
    printf("-0.0 is %s\n", show_classification(-0.000000));
    printf("1.0 is %s\n", show_classification(1.000000));
}


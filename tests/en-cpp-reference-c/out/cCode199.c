struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


int __cdecl feclearexcept(int _Flags);
double __cdecl sqrt(double _X);
int __cdecl fetestexcept(int _Flags);
double __cdecl frexp(double _X, int * _Y);
double __cdecl fabs(double _X);
double __cdecl scalbn(double _X, int _Y);

double hypot_demo(double a, double b)
{
    int range_problem;
    double result;
    int a_exponent;
    int b_exponent;
    double a_scaled;
    double b_scaled;

    range_problem = 6;
    feclearexcept(6);
    result = sqrt(a * a + b * b);
    if (!fetestexcept(6))
    {
        return result;
    }
    frexp(a, &a_exponent);
    frexp(b, &b_exponent);
    if (a_exponent - b_exponent > 1024)
    {
        return fabs(a) + fabs(b);
    }
    a_scaled = scalbn(a, -a_exponent);
    b_scaled = scalbn(b, -a_exponent);
    result = sqrt(a_scaled * a_scaled + b_scaled * b_scaled);
    return scalbn(result, a_exponent);
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
    printf("hypot(%f, %f) = %f\n", 3.000000, 4.000000, hypot_demo(3.000000, 4.000000));
    printf("hypot(%e, %e) = %e\n", 89884656743115785407263711865852178399035283762922498299458738401578630390014269380294779316383439085770229476757191232117160663444732091384233773351768758493024955288275641038122745045194664472037934254227566971152291618451611474082904279666061674137398913102072361584369088590459649940625202013092062429184.000000, 89884656743115785407263711865852178399035283762922498299458738401578630390014269380294779316383439085770229476757191232117160663444732091384233773351768758493024955288275641038122745045194664472037934254227566971152291618451611474082904279666061674137398913102072361584369088590459649940625202013092062429184.000000, hypot_demo(89884656743115785407263711865852178399035283762922498299458738401578630390014269380294779316383439085770229476757191232117160663444732091384233773351768758493024955288275641038122745045194664472037934254227566971152291618451611474082904279666061674137398913102072361584369088590459649940625202013092062429184.000000, 89884656743115785407263711865852178399035283762922498299458738401578630390014269380294779316383439085770229476757191232117160663444732091384233773351768758493024955288275641038122745045194664472037934254227566971152291618451611474082904279666061674137398913102072361584369088590459649940625202013092062429184.000000));
    return 0;
}


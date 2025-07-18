struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


double __cdecl sqrt(double _X);

double stddev(int count, ...)
{
    double sum;
    double sum_sq;
    char * args;

    sum = 0;
    sum_sq = 0;
    ((void)(args = (char *)(&(count)) + 4U));
    {
        int i;
        i = 0;
        for (; i < count; ++i)
        {
            double num;

            num = (*(double *)((args += 8U) - 8U));
            sum += num;
            sum_sq += num * num;
        }
    }
    ((void)(args = 0U));
    return sqrt(sum_sq / count - (sum / count) * (sum / count));
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
    printf("%f\n", stddev(4, 25.000000, 27.300000, 26.900000, 25.700000));
}


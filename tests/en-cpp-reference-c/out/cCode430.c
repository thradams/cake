struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


double __cdecl sqrt(double _X);

double sample_stddev(int count, ...)
{
    double sum;
    char * args1;
    char * args2;
    double mean;
    double sum_sq_diff;

    sum = 0;
    ((void)(args1 = (char *)(&(count)) + 4U));
    ((args2) = (args1));
    {
        int i;
        i = 0;
        for (; i < count; ++i)
        {
            double num;

            num = (*(double *)((args1 += 8U) - 8U));
            sum += num;
        }
    }
    ((void)(args1 = 0U));
    mean = sum / count;
    sum_sq_diff = 0;
    {
        int i;
        i = 0;
        for (; i < count; ++i)
        {
            double num;

            num = (*(double *)((args2 += 8U) - 8U));
            sum_sq_diff += (num - mean) * (num - mean);
        }
    }
    ((void)(args2 = 0U));
    return sqrt(sum_sq_diff / count);
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
    printf("%f\n", sample_stddev(4, 25.000000, 27.300000, 26.900000, 25.700000));
}


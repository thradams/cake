/* Cake 0.12.26 x86_msvc */
struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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
    ((void)(args1 = (char *)(&(count)) + 4));
    ((args2) = (args1));
    {
        int i;

        i = 0;
        for (; i < count; ++i)
        {
            double num;

            num = (*(double *)((args1 += 8) - 8));
            sum += num;
        }
    }
    ((void)(args1 = (char *)0));
    mean = sum / count;
    sum_sq_diff = 0;
    {
        int i;

        i = 0;
        for (; i < count; ++i)
        {
            double num;

            num = (*(double *)((args2 += 8) - 8));
            sum_sq_diff += (num - mean) * (num - mean);
        }
    }
    ((void)(args2 = (char *)0));
    return sqrt(sum_sq_diff / count);
}


static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    printf("%f\n", sample_stddev(4, 25.0, 27.300000000000001, 26.899999999999999, 25.699999999999999));
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

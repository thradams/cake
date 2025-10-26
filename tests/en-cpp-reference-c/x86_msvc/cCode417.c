/* Cake 0.12.26 x86_msvc */
struct _timespec64 {
    long long tv_sec;
    long tv_nsec;
};

struct timespec {
    long long tv_sec;
    long tv_nsec;
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


static double __cdecl difftime(long long _Time1, long long _Time2);
double __cdecl _difftime64(long long _Time1, long long _Time2);

double usdiff(struct timespec  s, struct timespec  e)
{
    double sdiff;
    long nsdiff;

    sdiff = difftime(e.tv_sec, s.tv_sec);
    nsdiff = e.tv_nsec - s.tv_nsec;
    if (nsdiff < 0)
    {
        return 1000000 * (sdiff - 1) + (1000000000 + nsdiff) / 1000.0;
    }
    else
    {
        return 1000000 * (sdiff) + nsdiff / 1000.0;
    }
}



static double __cdecl difftime(long long _Time1, long long _Time2)
{
    return _difftime64(_Time1, _Time2);
}
static int __cdecl timespec_get(struct timespec * _Ts, int _Base);
int __cdecl _timespec64_get(struct _timespec64 * _Ts, int _Base);
void __cdecl thrd_yield(void);

void sleep_100us()
{
    struct timespec  start;
    struct timespec  end;

    timespec_get(&start, 1);
    do
    {
        thrd_yield();
        timespec_get(&end, 1);
    }
    while (usdiff(start, end) < 100.0);
}



static int __cdecl timespec_get(struct timespec * _Ts, int _Base)
{
    return _timespec64_get((struct _timespec64 *)_Ts, _Base);
}
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main()
{
    struct timespec  start;
    struct timespec  end;

    timespec_get(&start, 1);
    sleep_100us();
    timespec_get(&end, 1);
    printf("Waited for %.3f us\n", usdiff(start, end));
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

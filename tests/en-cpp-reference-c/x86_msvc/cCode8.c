/* Cake 0.12.26 x86_msvc */
struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
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


static long long __cdecl time(long long * _Time);
long long __cdecl _time64(long long * _Time);
static struct tm *__cdecl localtime(long long * _Time);
struct tm *__cdecl _localtime64(long long * _Time);
static double __cdecl difftime(long long _Time1, long long _Time2);
double __cdecl _difftime64(long long _Time1, long long _Time2);
static long long __cdecl mktime(struct tm * _Tm);
long long __cdecl _mktime64(struct tm * _Tm);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    long long now;
    struct tm  beg;
    double seconds;

    now = time(0);
    beg = *localtime(&now);
    beg.tm_hour = 0, beg.tm_min = 0, beg.tm_sec = 0, beg.tm_mday = 1;
    seconds = difftime(now, mktime(&beg));
    printf("%.f seconds have passed since the beginning of the month.\n", seconds);
    return 0;
}



static long long __cdecl time(long long * _Time)
{
    return _time64(_Time);
}

static struct tm *__cdecl localtime(long long * _Time)
{
    return _localtime64(_Time);
}

static double __cdecl difftime(long long _Time1, long long _Time2)
{
    return _difftime64(_Time1, _Time2);
}

static long long __cdecl mktime(struct tm * _Tm)
{
    return _mktime64(_Tm);
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

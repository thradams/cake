/* Cake 0.12.26 x86_msvc */
struct _timespec64 {
    long long tv_sec;
    long tv_nsec;
};

struct timespec {
    long long tv_sec;
    long tv_nsec;
};

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


static int __cdecl timespec_get(struct timespec * _Ts, int _Base);
int __cdecl _timespec64_get(struct _timespec64 * _Ts, int _Base);
unsigned int __cdecl strftime(char * _Buffer, unsigned int _SizeInBytes, char * _Format, struct tm * _Tm);
static struct tm *__cdecl gmtime(long long * _Time);
struct tm *__cdecl _gmtime64(long long * _Time);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    struct timespec  ts;
    char buff[100];

    timespec_get(&ts, 1);
    strftime(buff, 100, "%D %T", gmtime(&ts.tv_sec));
    printf("Current time: %s.%09ld UTC\n", buff, ts.tv_nsec);
    printf("Raw timespec.time_t: %jd\n", (long long)ts.tv_sec);
    printf("Raw timespec.tv_nsec: %09ld\n", ts.tv_nsec);
}



static int __cdecl timespec_get(struct timespec * _Ts, int _Base)
{
    return _timespec64_get((struct _timespec64 *)_Ts, _Base);
}

static struct tm *__cdecl gmtime(long long * _Time)
{
    return _gmtime64(_Time);
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

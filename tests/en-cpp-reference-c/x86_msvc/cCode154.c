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


unsigned int __cdecl strftime(char * _Buffer, unsigned int _SizeInBytes, char * _Format, struct tm * _Tm);
static struct tm *__cdecl localtime(long long * _Time);
struct tm *__cdecl _localtime64(long long * _Time);
static long long __cdecl time(long long * _Time);
long long __cdecl _time64(long long * _Time);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
static int __cdecl vprintf(char * _Format, char * _ArgList);

void tlog(char * fmt, ...)
{
    char msg[50];
    long long __v0;
    char * args;

    __v0(null) = time(0);
    strftime(msg, 50, "%T", localtime(&__v0));
    printf("[%s] ", msg);
    ((void)(args = (char *)(&(fmt)) + 4));
    vprintf(fmt, args);
    ((void)(args = (char *)0));
}



static struct tm *__cdecl localtime(long long * _Time)
{
    return _localtime64(_Time);
}

static long long __cdecl time(long long * _Time)
{
    return _time64(_Time);
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

static int __cdecl vprintf(char * _Format, char * _ArgList)
{
    return _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
}
int main(void)
{
    tlog("logging %d %d %d...\n", 1, 2, 3);
}



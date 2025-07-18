struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};

struct timespec {
    __int64 tv_sec;
    long tv_nsec;
};

struct _timespec64 {
    __int64 tv_sec;
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



int __cdecl _timespec64_get(struct _timespec64 * _Ts, int _Base);
inline static int __cdecl timespec_get(struct timespec * _Ts, int _Base)
{
    return _timespec64_get((struct _timespec64 *)_Ts, _Base);
}
unsigned int __cdecl strftime(char * _Buffer, unsigned int _SizeInBytes, char * _Format, struct tm * _Tm);

struct tm *__cdecl _gmtime64(__int64 * _Time);
inline static struct tm *__cdecl gmtime(__int64 * _Time)
{
    return _gmtime64(_Time);
}


int __cdecl __stdio_common_vsprintf(unsigned __int64 _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

static unsigned __int64 __ck__OptionsStorage1;
inline unsigned __int64 *__cdecl __local_stdio_printf_options(void)
{
    return &__ck__OptionsStorage1;
}
inline int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList)
{
    int _Result;

    _Result = __stdio_common_vsprintf((*__local_stdio_printf_options()) | 2ULL, _Buffer, _BufferCount, _Format, 0U, _ArgList);
    return _Result < 0 ? -1 : _Result;
}
inline int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}


int __cdecl __stdio_common_vfprintf(unsigned __int64 _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
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

void debug_log(char * fmt, ...)
{
    struct timespec  ts;
    char time_buf[100];
    unsigned int rc;
    char * args1;
    char * args2;
    char buf[];

    timespec_get(&ts, 1);
    rc = strftime(time_buf, 100U, "%D %T", gmtime(&ts.tv_sec));
    snprintf(time_buf + rc, 100U - rc, ".%06ld UTC", ts.tv_nsec / 1000);
    ((void)(args1 = (char *)(&(fmt)) + 4U));
    ((args2) = (args1));
    ((void)(args1 = 0U));
    vsnprintf(buf, 0, fmt, args2);
    ((void)(args2 = 0U));
    printf("%s [debug]: %s\n", time_buf, buf);
}

int main(void)
{
    debug_log("Logging, %d, %d, %d", 1, 2, 3);
}


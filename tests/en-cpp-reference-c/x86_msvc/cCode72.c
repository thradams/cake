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


__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;
static int __cdecl timespec_get(struct timespec * _Ts, int _Base);
int __cdecl _timespec64_get(struct _timespec64 * _Ts, int _Base);
unsigned int __cdecl strftime(char * _Buffer, unsigned int _SizeInBytes, char * _Format, struct tm * _Tm);
static struct tm *__cdecl gmtime(long long * _Time);
struct tm *__cdecl _gmtime64(long long * _Time);
static int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);
static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList);
int __cdecl __stdio_common_vsprintf(unsigned long long _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
static int __cdecl vswprintf(unsigned short * _Buffer, unsigned int _BufferCount, unsigned short * _Format, char * _ArgList);
static int __cdecl _vswprintf_c_l(unsigned short * _Buffer, unsigned int _BufferCount, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vswprintf(unsigned long long _Options, unsigned short * _Buffer, unsigned int _BufferCount, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static int __cdecl wprintf(unsigned short * _Format, ...);
static int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfwprintf(unsigned long long _Options, struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void debug_wlog(unsigned short * fmt, ...)
{
    struct timespec  ts;
    char time_buf[100];
    unsigned int rc;
    char * args;
    unsigned short buf[1024];
    int rc2;

    timespec_get(&ts, 1);
    rc = strftime(time_buf, 100, "%D %T", gmtime(&ts.tv_sec));
    snprintf(time_buf + rc, 100 - rc, ".%06ld UTC", ts.tv_nsec / 1000);
    ((void)(args = (char *)(&(fmt)) + 4));
    rc2 = vswprintf(buf, 1024, fmt, args);
    ((void)(args = (char *)0));
    if (rc2 > 0)
    {
        wprintf(L"%s [debug]: %ls\n", time_buf, buf);
    }
    else
    {
        wprintf(L"%s [debug]: (string too long)\n", time_buf);
    }
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

static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList)
{
    int _Result;

    _Result = __stdio_common_vsprintf((*__local_stdio_printf_options()) | 2ULL, _Buffer, _BufferCount, _Format, 0, _ArgList);
    return _Result < 0 ? -1 : _Result;
}

static int __cdecl snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

static int __cdecl _vswprintf_c_l(unsigned short * _Buffer, unsigned int _BufferCount, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    int _Result;

    _Result = __stdio_common_vswprintf((*__local_stdio_printf_options()), _Buffer, _BufferCount, _Format, _Locale, _ArgList);
    return _Result < 0 ? -1 : _Result;
}

static int __cdecl vswprintf(unsigned short * _Buffer, unsigned int _BufferCount, unsigned short * _Format, char * _ArgList)
{
    return _vswprintf_c_l(_Buffer, _BufferCount, _Format, 0, _ArgList);
}

static int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfwprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl wprintf(unsigned short * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vfwprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}
char *__cdecl setlocale(int _Category, char * _Locale);

int main(void)
{
    setlocale(0, "");
    debug_wlog(L"Logging, %d, %d, %d", 1, 2, 3);
}



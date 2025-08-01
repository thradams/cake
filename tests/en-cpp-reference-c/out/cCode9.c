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

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};



__int64 __cdecl _time64(__int64 * _Time);
inline static __int64 __cdecl time(__int64 * _Time)
{
    return _time64(_Time);
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
char *__cdecl asctime(struct tm * _Tm);

struct tm *__cdecl _gmtime64(__int64 * _Time);
inline static struct tm *__cdecl gmtime(__int64 * _Time)
{
    return _gmtime64(_Time);
}

struct tm *__cdecl _localtime64(__int64 * _Time);
inline static struct tm *__cdecl localtime(__int64 * _Time)
{
    return _localtime64(_Time);
}
int __cdecl putenv(char * _EnvString);

int main(void)
{
    __int64 t;

    t = time(0U);
    printf("UTC:       %s", asctime(gmtime(&t)));
    printf("local:     %s", asctime(localtime(&t)));
    putenv("TZ=Asia/Singapore");
    printf("Singapore: %s", asctime(localtime(&t)));
}


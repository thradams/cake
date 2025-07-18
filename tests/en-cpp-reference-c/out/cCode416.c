struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};

struct _timespec64 {
    __int64 tv_sec;
    long tv_nsec;
};

struct timespec {
    __int64 tv_sec;
    long tv_nsec;
};




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

char *__cdecl _ctime64(__int64 * _Time);
inline static char *__cdecl ctime(__int64 * _Time)
{
    return _ctime64(_Time);
}

__int64 __cdecl _time64(__int64 * _Time);
inline static __int64 __cdecl time(__int64 * _Time)
{
    return _time64(_Time);
}

int __cdecl _thrd_sleep64(struct _timespec64 * duration, struct _timespec64 * remaining);
inline static int __cdecl thrd_sleep(struct timespec * duration, struct timespec * remaining)
{
    return _thrd_sleep64((struct _timespec64 *)duration, (struct _timespec64 *)remaining);
}

int main(void)
{
    __int64 __cmp_lt_0;
    struct timespec  __cmp_lt_1;
    __int64 __cmp_lt_2;

    __cmp_lt_0(null) = time(0U);
    printf("Time: %s", ctime(&__cmp_lt_0));
    __cmp_lt_1.tv_sec = 1;
    __cmp_lt_1.tv_nsec = 0;
    thrd_sleep(&__cmp_lt_1, 0U);
    __cmp_lt_2(null) = time(0U);
    printf("Time: %s", ctime(&__cmp_lt_2));
}


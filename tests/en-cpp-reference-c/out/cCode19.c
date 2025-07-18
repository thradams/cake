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

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


int _Avx2WmemEnabledWeakValue = 0;
unsigned int __cdecl wcsftime(unsigned short * _Buffer, unsigned int _SizeInWords, unsigned short * _Format, struct tm * _Tm);


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
int __cdecl puts(char * _Buffer);
char *__cdecl setlocale(int _Category, char * _Locale);

int main(void)
{
    unsigned short buff[40];
    struct tm  my_time;

    my_time.tm_sec = 20;
    my_time.tm_min = 10;
    my_time.tm_hour = 8;
    my_time.tm_mday = 9;
    my_time.tm_mon = 9;
    my_time.tm_year = 112;
    my_time.tm_wday = 0;
    my_time.tm_yday = 0;
    my_time.tm_isdst = 0;
    if (wcsftime(buff, 80U, L"%A %c", &my_time))
    {
        printf("%ls\n", buff);
    }
    else
    {
        puts("wcsftime failed");
    }
    setlocale(0, "ja_JP.utf8");
    if (wcsftime(buff, 80U, L"%A %c", &my_time))
    {
        printf("%ls\n", buff);
    }
    else
    {
        puts("wcsftime failed");
    }
}


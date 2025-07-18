struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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

void signal_handler(int sig)
{
    printf("Received signal: %d\n", sig);
}

void (__cdecl *__cdecl signal(int _Signal, void (__cdecl * _Function)(int)))(int);
void __cdecl exit(int _Code);
int __cdecl raise(int _Signal);

int main(void)
{
    if (signal(15, signal_handler) == 4294967295U)
    {
        printf("Error while installing a signal handler.\n");
        exit(1);
    }
    printf("Sending signal: %d\n", 15);
    if (raise(15) != 0)
    {
        printf("Error while raising the SIGTERM signal.\n");
        exit(1);
    }
    printf("Exit main()\n");
    return 0;
}


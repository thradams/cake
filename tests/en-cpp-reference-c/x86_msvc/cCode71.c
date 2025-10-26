/* Cake 0.12.26 x86_msvc */
struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


static int __cdecl vsscanf(char * _Buffer, char * _Format, char * _ArgList);
static int __cdecl _vsscanf_l(char * _Buffer, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vsscanf(unsigned long long _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_scanf_options(void);
static unsigned long long __c0__OptionsStorage;

unsigned char checked_sscanf(int count, char * buf, char * fmt, ...)
{
    char * ap;
    int rc;

    ((void)(ap = (char *)(&(fmt)) + 4));
    rc = vsscanf(buf, fmt, ap);
    ((void)(ap = (char *)0));
    return rc == count;
}



static unsigned long long *__cdecl __local_stdio_scanf_options(void)
{
    return &__c0__OptionsStorage;
}

static int __cdecl _vsscanf_l(char * _Buffer, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vsscanf((*__local_stdio_scanf_options()), _Buffer, 4294967295, _Format, _Locale, _ArgList);
}

static int __cdecl vsscanf(char * _Buffer, char * _Format, char * _ArgList)
{
    return _vsscanf_l(_Buffer, _Format, 0, _ArgList);
}
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c1__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl puts(char * _Buffer);

int main(void)
{
    int n;
    int m;

    printf("Parsing '1 2'...");
    if (checked_sscanf(2, "1 2", "%d %d", &n, &m))
    {
        puts("success");
    }
    else
    {
        puts("failure");
    }
    printf("Parsing '1 a'...");
    if (checked_sscanf(2, "1 a", "%d %d", &n, &m))
    {
        puts("success");
    }
    else
    {
        puts("failure");
    }
}



static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__c1__OptionsStorage;
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

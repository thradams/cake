/* Cake x64_msvc */

struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);
unsigned long long __cdecl fwrite(void * _Buffer, unsigned long long _ElementSize, unsigned long long _ElementCount, struct _iobuf * _Stream);
static int __cdecl printf(char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl fclose(struct _iobuf * _Stream);
unsigned long long __cdecl fread(void * _Buffer, unsigned long long _ElementSize, unsigned long long _ElementCount, struct _iobuf * _Stream);

int main(void)
{
    double a[5];
    struct _iobuf * f1;
    unsigned long long r1;
    double b[5];
    struct _iobuf * f2;
    unsigned long long r2;

    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
    a[3] = 4;
    a[4] = 5;
    f1 = fopen("file.bin", "wb");
    ;
    r1 = fwrite(a, 8ULL, 5, f1);
    printf("wrote %zu elements out of %d requested\n", r1, 5);
    fclose(f1);
    f2 = fopen("file.bin", "rb");
    r2 = fread(b, 8ULL, 5, f2);
    fclose(f2);
    printf("read back: ");
    {
        unsigned long long i;

        i = 0;
        for (; i < r2; ++i)
        {
            printf("%0.2f ", b[i]);
        }
    }
}

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &_OptionsStorage;
}

static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(__va_start(&_ArgList, _Format)));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

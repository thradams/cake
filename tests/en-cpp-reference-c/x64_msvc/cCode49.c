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
int __cdecl fclose(struct _iobuf * _Stream);
int __cdecl fseek(struct _iobuf * _Stream, long _Offset, int _Origin);
static int __cdecl fprintf(struct _iobuf * _Stream, char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
unsigned long long __cdecl fread(void * _Buffer, unsigned long long _ElementSize, unsigned long long _ElementCount, struct _iobuf * _Stream);
static int __cdecl printf(char * _Format, ...);

int main(void)
{
    double A[5];
    struct _iobuf * fp;
    double B[5];
    int ret_code;

    A[0] = 1.0;
    A[1] = 2.0;
    A[2] = 3.0;
    A[3] = 4.0;
    A[4] = 5.0;
    fp = fopen("test.bin", "wb");
    fwrite(A, 8ULL, 5, fp);
    fclose(fp);
    fp = fopen("test.bin", "rb");
    if (fseek(fp, 16ULL, 0) != 0)
    {
        fprintf((__acrt_iob_func(2)), "fseek() failed in file %s at line # %d\n", "c:/users/thiago/source/repos/cake_private/tests/en-cpp-reference-c/ccode49.c", 20);
        fclose(fp);
        return 1;
    }
    ret_code = fread(B, 8ULL, 1, fp);
    printf("ret_code == %d\n", ret_code);
    printf("B[0] == %.1f\n", B[0]);
    fclose(fp);
    return 0;
}

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &_OptionsStorage;
}

static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl fprintf(struct _iobuf * _Stream, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(__va_start(&_ArgList, _Format)));
    _Result = _vfprintf_l(_Stream, _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
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

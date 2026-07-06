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
int __cdecl fgetpos(struct _iobuf * _Stream, long long * _Position);
unsigned long long __cdecl fread(void * _Buffer, unsigned long long _ElementSize, unsigned long long _ElementCount, struct _iobuf * _Stream);
static int __cdecl printf(char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl fsetpos(struct _iobuf * _Stream, long long * _Position);
void __cdecl perror(char * _ErrorMessage);

int main(void)
{
    struct _iobuf * fp;
    int rc;
    double __a[4];
    long long pos;
    double d;

    fp = fopen("test.bin", "wb");
    ;
    __a[0] = 1.1000000000000001;
    __a[1] = 2.2000000000000002;
    __a[2] = 3.2999999999999998;
    __a[3] = 4.4000000000000004;
    rc = fwrite(__a, 8ULL, 4, fp);
    ;
    fclose(fp);
    fp = fopen("test.bin", "rb");
    fgetpos(fp, &pos);
    rc = fread(&d, 8ULL, 1, fp);
    ;
    printf("First value in the file: %.1f\n", d);
    fsetpos(fp, &pos);
    rc = fread(&d, 8ULL, 1, fp);
    ;
    printf("First value in the file again: %.1f\n", d);
    fclose(fp);
    rc = fsetpos((__acrt_iob_func(0)), &pos);
    if (rc)
    {
        perror("could not fsetpos stdin");
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

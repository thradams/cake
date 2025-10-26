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


struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);
unsigned int __cdecl fwrite(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);
int __cdecl fclose(struct _iobuf * _Stream);
int __cdecl fgetpos(struct _iobuf * _Stream, long long * _Position);
unsigned int __cdecl fread(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl fsetpos(struct _iobuf * _Stream, long long * _Position);
void __cdecl perror(char * _ErrorMessage);

int main(void)
{
    struct _iobuf * fp;
    int rc;
    double __v0[4];
    long long pos;
    double d;

    fp = fopen("test.bin", "wb");
    ;
    __v0[0] = 1.1000000000000001;
    __v0[1] = 2.2000000000000002;
    __v0[2] = 3.2999999999999998;
    __v0[3] = 4.4000000000000004;
    rc = fwrite(__v0, 8, 4, fp);
    ;
    fclose(fp);
    fp = fopen("test.bin", "rb");
    fgetpos(fp, &pos);
    rc = fread(&d, 8, 1, fp);
    ;
    printf("First value in the file: %.1f\n", d);
    fsetpos(fp, &pos);
    rc = fread(&d, 8, 1, fp);
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
    return &__c0__OptionsStorage;
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

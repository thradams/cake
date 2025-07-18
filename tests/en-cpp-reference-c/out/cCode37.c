struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);
unsigned int __cdecl fwrite(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);
int __cdecl fclose(struct _iobuf * _Stream);
int __cdecl fgetpos(struct _iobuf * _Stream, __int64 * _Position);
unsigned int __cdecl fread(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);


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
int __cdecl fsetpos(struct _iobuf * _Stream, __int64 * _Position);
void __cdecl perror(char * _ErrorMessage);

int main(void)
{
    struct _iobuf * fp;
    int rc;
    double __cmp_lt_0[4];
    __int64 pos;
    double d;

    fp = fopen("test.bin", "wb");
    ;
    __cmp_lt_0[0] = 1.100000;
    __cmp_lt_0[1] = 2.200000;
    __cmp_lt_0[2] = 3.300000;
    __cmp_lt_0[3] = 4.400000;
    rc = fwrite(__cmp_lt_0, 8U, 4, fp);
    ;
    fclose(fp);
    fp = fopen("test.bin", "rb");
    fgetpos(fp, &pos);
    rc = fread(&d, 8U, 1, fp);
    ;
    printf("First value in the file: %.1f\n", d);
    fsetpos(fp, &pos);
    rc = fread(&d, 8U, 1, fp);
    ;
    printf("First value in the file again: %.1f\n", d);
    fclose(fp);
    rc = fsetpos((__acrt_iob_func(0)), &pos);
    if (rc)
    {
        perror("could not fsetpos stdin");
    }
}


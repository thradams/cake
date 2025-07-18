struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);
unsigned int __cdecl fwrite(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);
int __cdecl fclose(struct _iobuf * _Stream);
int __cdecl fgetpos(struct _iobuf * _Stream, __int64 * _Position);
void __cdecl perror(char * _ErrorMessage);


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
inline int __cdecl fprintf(struct _iobuf * _Stream, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfprintf_l(_Stream, _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
void __cdecl exit(int _Code);
unsigned int __cdecl fread(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);

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
int __cdecl ferror(struct _iobuf * _Stream);

int main(void)
{
    double A[5];
    struct _iobuf * fp;
    double B[5];
    __int64 pos;
    int ret_code;

    A[0] = 1.000000;
    A[1] = 2.000000;
    A[2] = 3.000000;
    A[3] = 4.000000;
    A[4] = 5.000000;
    fp = fopen("test.bin", "wb");
    fwrite(A, 8U, 5, fp);
    fclose(fp);
    fp = fopen("test.bin", "rb");
    if (fgetpos(fp, &pos) != 0)
    {
        perror("fgetpos()");
        fprintf((__acrt_iob_func(2)), "fgetpos() failed in file %s at line # %d\n", "c:/users/thiag/source/repos/cake/tests/en-cpp-reference-c/ccode50.c", 19);
        exit(1);
    }
    ret_code = fread(B, 8U, 1, fp);
    printf("%.1f; read count = %d\n", B[0], ret_code);
    if (fsetpos(fp, &pos) != 0)
    {
        if (ferror(fp))
        {
            perror("fsetpos()");
            fprintf((__acrt_iob_func(2)), "fsetpos() failed in file %s at line # %d\n", "c:/users/thiag/source/repos/cake/tests/en-cpp-reference-c/ccode50.c", 30);
            exit(1);
        }
    }
    ret_code = fread(B, 8U, 1, fp);
    printf("%.1f; read count = %d\n", B[0], ret_code);
    fclose(fp);
    return 0;
}


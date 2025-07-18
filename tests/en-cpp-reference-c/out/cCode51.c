struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


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

void check(unsigned char   condition, char * func, int line)
{
    if (condition)
    {
        return;
    }
    perror(func);
    fprintf((__acrt_iob_func(2)), "%s failed in file %s at line # %d\n", func, "c:/users/thiag/source/repos/cake/tests/en-cpp-reference-c/ccode51.c", line - 1);
    exit(1);
}

struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);
unsigned int __cdecl fwrite(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);
int __cdecl fclose(struct _iobuf * _Stream);
long __cdecl ftell(struct _iobuf * _Stream);

inline int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}
unsigned int __cdecl fread(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);

int main(void)
{
    double A[5];
    char * fname;
    struct _iobuf * file;
    int write_count;
    double B[5];
    int long pos;
    int read_count;

    A[0] = 1.100000;
    A[1] = 2.000000;
    A[2] = 3.000000;
    A[3] = 4.000000;
    A[4] = 5.000000;
    fname = "/tmp/test.bin";
    file = fopen(fname, "wb");
    check(file != 0U, "fopen()", 24);
    write_count = fwrite(A, 8U, 5, file);
    check(write_count == 5, "fwrite()", 27);
    fclose(file);
    file = fopen(fname, "rb");
    check(file != 0U, "fopen()", 34);
    pos = ftell(file);
    check(pos != -1L, "ftell()", 37);
    printf("pos: %ld\n", pos);
    read_count = fread(B, 8U, 1, file);
    check(read_count == 1, "fread()", 41);
    pos = ftell(file);
    check(pos != -1L, "ftell()", 44);
    printf("pos: %ld\n", pos);
    printf("B[0]: %.1f\n", B[0]);
    return 0;
}


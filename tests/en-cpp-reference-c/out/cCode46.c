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
struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);
unsigned int __cdecl fwrite(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);
int __cdecl fclose(struct _iobuf * _Stream);
unsigned int __cdecl fread(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);
int __cdecl putchar(int _Character);
int __cdecl feof(struct _iobuf * _Stream);
int __cdecl ferror(struct _iobuf * _Stream);
void __cdecl perror(char * _ErrorMessage);

int main(void)
{
    double a[5];
    struct _iobuf * fp;
    double b[5];
    unsigned int ret_code;

    a[0] = 1.000000;
    a[1] = 2.000000;
    a[2] = 3.000000;
    a[3] = 4.000000;
    a[4] = 5.000000;
    printf("Array has size %ld bytes, element size: %ld\n", 40U, 8U);
    fp = fopen("test.bin", "wb");
    fwrite(a, 8U, 5, fp);
    fclose(fp);
    fp = fopen("test.bin", "rb");
    ret_code = fread(b, 8U, 5, fp);
    if (ret_code == 5)
    {
        printf("Array at %p read successfully, contents:\n", (void *)&a);
        {
            int n;
            n = 0;
            for (; n != 5; ++n)
            printf("%f ", b[n]);
        }
        putchar(10);
    }
    else
    {
        if (feof(fp))
        {
            printf("Error reading test.bin: unexpected end of file\n");
        }
        else
        {
            if (ferror(fp))
            {
                perror("Error reading test.bin");
            }
        }
    }
    fclose(fp);
}


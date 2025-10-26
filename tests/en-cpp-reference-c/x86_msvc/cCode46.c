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


static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
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

    a[0] = 1.0;
    a[1] = 2.0;
    a[2] = 3.0;
    a[3] = 4.0;
    a[4] = 5.0;
    printf("Array has size %ld bytes, element size: %ld\n", 40, 8);
    fp = fopen("test.bin", "wb");
    fwrite(a, 8, 5, fp);
    fclose(fp);
    fp = fopen("test.bin", "rb");
    ret_code = fread(b, 8, 5, fp);
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

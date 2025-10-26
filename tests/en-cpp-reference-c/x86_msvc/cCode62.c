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
void __cdecl perror(char * _ErrorMessage);
int __cdecl fputc(int _Character, struct _iobuf * _Stream);
int __cdecl fclose(struct _iobuf * _Stream);
int __cdecl rename(char * _OldFileName, char * _NewFileName);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl fgetc(struct _iobuf * _Stream);

int main(void)
{
    struct _iobuf * fp;
    int rc;

    fp = fopen("from.txt", "w");
    if (!fp)
    {
        perror("from.txt");
        return 1;
    }
    fputc(97, fp);
    fclose(fp);
    rc = rename("from.txt", "to.txt");
    if (rc)
    {
        perror("rename");
        return 1;
    }
    fp = fopen("to.txt", "r");
    if (!fp)
    {
        perror("to.txt");
        return 1;
    }
    printf("%c\n", fgetc(fp));
    fclose(fp);
    return 0;
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

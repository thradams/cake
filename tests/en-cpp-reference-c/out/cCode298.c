struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);


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
int __cdecl fclose(struct _iobuf * _Stream);

inline int __cdecl printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}

int main(void)
{
    struct _iobuf * fp;

    fp = fopen("data.txt", "r");
    if (fp == 0U)
    {
        fprintf((__acrt_iob_func(2)), "error opening file data.txt in function main()\n");
        exit(1);
    }
    fclose(fp);
    printf("Normal Return\n");
    return 0;
}


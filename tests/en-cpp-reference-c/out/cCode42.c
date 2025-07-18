struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


int __cdecl putc(int _Character, struct _iobuf * _Stream);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl ferror(struct _iobuf * _Stream);
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
void __cdecl exit(int _Code);

int main(void)
{
    int ret_code;

    ret_code = 0;
    {
        char c;
        c = 97;
        for (; (ret_code != -1) && (c != 122); c++)
        ret_code = putc(c, (__acrt_iob_func(1)));
    }
    if (ret_code == -1 && ferror((__acrt_iob_func(1))))
    {
        perror("putc()");
        fprintf((__acrt_iob_func(2)), "putc() failed in file %s at line # %d\n", "c:/users/thiag/source/repos/cake/tests/en-cpp-reference-c/ccode42.c", 9);
        exit(1);
    }
    putc(10, (__acrt_iob_func(1)));
    return 0;
}


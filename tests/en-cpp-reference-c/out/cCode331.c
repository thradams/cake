struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


char *__cdecl setlocale(int _Category, char * _Locale);


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
int __cdecl strcoll(char * _String1, char * _String2);
int __cdecl strcmp(char * _Str1, char * _Str2);

int main(void)
{
    char * s1;
    char * s2;

    setlocale(1, "cs_CZ.utf8");
    s1 = "hrnec";
    s2 = "chrt";
    printf("In the Czech locale: ");
    if (strcoll(s1, s2) < 0)
    {
        printf("%s before %s\n", s1, s2);
    }
    else
    {
        printf("%s before %s\n", s2, s1);
    }
    printf("In lexicographical comparison: ");
    if (strcmp(s1, s2) < 0)
    {
        printf("%s before %s\n", s1, s2);
    }
    else
    {
        printf("%s before %s\n", s2, s1);
    }
}


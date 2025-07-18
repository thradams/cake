struct _iobuf {
    void * _Placeholder;
};

struct _Mbstatet {
    unsigned long _Wchar;
    unsigned short _Byte;
    unsigned short _State;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


int _Avx2WmemEnabledWeakValue = 0;
char *__cdecl setlocale(int _Category, char * _Locale);
unsigned int __cdecl strlen(char * _Str);
void *__cdecl memset(void * _Dst, int _Val, unsigned int _Size);
unsigned int __cdecl mbrlen(char * _Ch, unsigned int _SizeInBytes, struct _Mbstatet * _State);


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

int main(void)
{
    char * str;
    unsigned int sz;
    struct _Mbstatet  mb;
    int len1;
    int len2;

    setlocale(0, "en_US.utf8");
    str = "\xe6\xb0\xb4";
    sz = strlen(str);
    memset(&mb, 0, 8U);
    len1 = mbrlen(str, 1, &mb);
    if (len1 == -2)
    {
        printf("The first 1 byte of %s is an incomplete multibyte char"" (mbrlen returns -2)\n", str);
    }
    len2 = mbrlen(str + 1, sz - 1, &mb);
    printf("The remaining %zu  bytes of %s hold %d bytes of the multibyte"" character\n", sz - 1, str, len2);
    printf("Attempting to call mbrlen() in the middle of %s while in initial"" shift state returns %zd\n", str, mbrlen(str + 1, sz - 1, &mb));
}


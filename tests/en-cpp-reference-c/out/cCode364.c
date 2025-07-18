struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};

struct _Mbstatet {
    unsigned long _Wchar;
    unsigned short _Byte;
    unsigned short _State;
};


int _Avx2WmemEnabledWeakValue = 0;
char *__cdecl setlocale(int _Category, char * _Locale);
unsigned int __cdecl mbrlen(char * _Ch, unsigned int _SizeInBytes, struct _Mbstatet * _State);
static struct _Mbstatet  __ck_mb1;

inline int __cdecl mbsinit(struct _Mbstatet * _P)
{
    return _P == 0U || _P->_Wchar == 0;
}


int __cdecl __stdio_common_vfprintf(unsigned __int64 _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

static unsigned __int64 __ck__OptionsStorage2;
inline unsigned __int64 *__cdecl __local_stdio_printf_options(void)
{
    return &__ck__OptionsStorage2;
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
unsigned int __cdecl strlen(char * _Str);

int main(void)
{
    char * str;

    setlocale(0, "en_US.utf8");
    str = "\xe6\xb0\xb4";
    (void)mbrlen(&str[0], 1, &__ck_mb1);
    if (!mbsinit(&__ck_mb1))
    {
        printf("After processing the first 1 byte of %s,\n""the conversion state is not initial\n\n", str);
    }
    (void)mbrlen(&str[1], strlen(str), &__ck_mb1);
    if (mbsinit(&__ck_mb1))
    {
        printf("After processing the remaining 2 bytes of %s,\n""the conversion state is initial conversion state\n", str);
    }
}


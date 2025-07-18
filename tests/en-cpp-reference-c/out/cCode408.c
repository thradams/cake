struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


int _Avx2WmemEnabledWeakValue = 0;
char *__cdecl setlocale(int _Category, char * _Locale);
unsigned int __cdecl wcsxfrm(unsigned short * _Destination, unsigned short * _Source, unsigned int _MaxCount);


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
int __cdecl wcscmp(unsigned short * _String1, unsigned short * _String2);

int main(void)
{
    unsigned short * in1;
    unsigned short out1[];
    unsigned short * in2;
    unsigned short out2[];

    setlocale(0, "sv_SE.utf8");
    in1 = L"\u00e5r";
    wcsxfrm(out1, in1, 0 / 2U);
    in2 = L"\u00e4ngel";
    wcsxfrm(out2, in2, 0 / 2U);
    printf("In the Swedish locale: ");
    if (wcscmp(out1, out2) < 0)
    {
        printf("%ls before %ls\n", in1, in2);
    }
    else
    {
        printf("%ls before %ls\n", in2, in1);
    }
    printf("In lexicographical comparison: ");
    if (wcscmp(in1, in2) < 0)
    {
        printf("%ls before %ls\n", in1, in2);
    }
    else
    {
        printf("%ls before %ls\n", in2, in1);
    }
}


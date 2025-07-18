struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


int _Avx2WmemEnabledWeakValue = 0;
int __cdecl iswctype(unsigned short _C, unsigned short _Type);
unsigned short __cdecl wctype(char * name);

char *classify(unsigned short wc, char * cat)
{
    return iswctype(wc, wctype(cat)) ? "true" : "false";
}

char *__cdecl setlocale(int _Category, char * _Locale);
int __cdecl puts(char * _Buffer);


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
    char *cats[5];

    setlocale(0, "ja_JP.UTF-8");
    puts("The character \u6c34 is...");
    cats[0] = "digit";
    cats[1] = "alpha";
    cats[2] = "space";
    cats[3] = "cntrl";
    cats[4] = "jkanji";
    {
        int n;
        n = 0;
        for (; n < 5; ++n)
        printf("%s?\t%s\n", cats[n], classify(27700, cats[n]));
    }
}


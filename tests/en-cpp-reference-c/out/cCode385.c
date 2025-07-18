struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


int _Avx2WmemEnabledWeakValue = 0;


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
int __cdecl iswctype(unsigned short _C, unsigned short _Type);
unsigned short __cdecl wctype(char * name);

void test(unsigned short a3, unsigned short u3, unsigned short j3)
{
    printf("\t  '%lc'  '%lc' '%lc'\n", a3, u3, j3);
    printf("iswxdigit: %d    %d    %d\n", !!(iswctype(a3, 128)), !!(iswctype(u3, 128)), !!(iswctype(j3, 128)));
    printf("jdigit:    %d    %d    %d\n", !!iswctype(a3, wctype("jdigit")), !!iswctype(u3, wctype("jdigit")), !!iswctype(j3, wctype("jdigit")));
}

char *__cdecl setlocale(int _Category, char * _Locale);
int __cdecl puts(char * _Buffer);

int main(void)
{
    unsigned short a3;
    unsigned short u3;
    unsigned short j3;

    a3 = 57;
    u3 = 12329;
    j3 = -231;
    setlocale(0, "en_US.utf8");
    puts("In American locale:");
    test(a3, u3, j3);
    setlocale(0, "ja_JP.utf8");
    puts("\nIn Japanese locale:");
    test(a3, u3, j3);
}


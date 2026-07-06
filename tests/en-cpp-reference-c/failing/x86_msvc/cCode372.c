/* Cake 0.13.29 x86_msvc */

struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

int __cdecl wctomb(char * _MbCh, unsigned short _WCh);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl ___mb_cur_max_func(void);

void demo(unsigned short wc)
{
    char * dep;
    unsigned int __vm0;
    char * mb;
    int len;

    dep = wctomb(0, wc) ? "Yes" : "No";
    printf("State-dependent encoding? %s.\n", dep);
    __vm0 = ___mb_cur_max_func();
    mb = _alloca(1 * __vm0);
    len = wctomb(mb, wc);
    printf("wide char '%lc' -> multibyte char [", wc);
    {
        int idx;

        idx = 0;
        for (; idx < len; ++idx)
        {
            printf("%s%#2x", idx ? " " : "", (unsigned char)mb[idx]);
        }
    }
    printf("]\n");
}

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &_OptionsStorage;
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

char *__cdecl setlocale(int _Category, char * _Locale);

int main(void)
{
    setlocale(0, "en_US.utf8");
    printf("MB_CUR_MAX = %zu\n", ___mb_cur_max_func());
    demo(65);
    demo(223);
    demo(53515);
}

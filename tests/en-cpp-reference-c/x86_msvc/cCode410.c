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


__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;
unsigned short __cdecl putwchar(unsigned short _Character);
static int __cdecl wmemcmp(unsigned short * _S1, unsigned short * _S2, unsigned int _N);
static int __cdecl wprintf(unsigned short * _Format, ...);
static int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfwprintf(unsigned long long _Options, struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void demo(unsigned short * lhs, unsigned short * rhs, unsigned int sz)
{
    int rc;

    {
        unsigned int n;

        n = 0;
        for (; n < sz; ++n)
        putwchar(lhs[n]);
    }
    rc = wmemcmp(lhs, rhs, sz);
    if (rc == 0)
    {
        wprintf(L" compares equal to ");
    }
    else
    {
        if (rc < 0)
        {
            wprintf(L" precedes ");
        }
        else
        {
            if (rc > 0)
            {
                wprintf(L" follows ");
            }
        }
    }
    {
        unsigned int n;

        n = 0;
        for (; n < sz; ++n)
        putwchar(rhs[n]);
    }
    wprintf(L" in lexicographical order\n");
}



static int __cdecl wmemcmp(unsigned short * _S1, unsigned short * _S2, unsigned int _N)
{
    unsigned int __count;

    __count = 0;
    for (; __count < _N; ++__count)
    {
        if (_S1[__count] != _S2[__count])
        {
            return _S1[__count] < _S2[__count] ? -1 : 1;
        }
    }
    return 0;
}

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__c0__OptionsStorage;
}

static int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfwprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl wprintf(unsigned short * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vfwprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}
char *__cdecl setlocale(int _Category, char * _Locale);

int main(void)
{
    unsigned short a1[3];
    unsigned short a2[3];
    unsigned int sz;

    setlocale(0, "en_US.utf8");
    a1[0] = 945;
    a1[1] = 946;
    a1[2] = 947;
    a2[0] = 945;
    a2[1] = 946;
    a2[2] = 948;
    sz = 3;
    demo(a1, a2, sz);
    demo(a2, a1, sz);
    demo(a1, a1, sz);
}



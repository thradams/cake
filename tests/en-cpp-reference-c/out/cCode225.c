struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};

struct _div_t {
    int quot;
    int rem;
};


void reverse(char * first, char * last)
{
    for (--last; first < last; ++first, --last)
    {
        char c;

        c = *last;
        *last = *first;
        *first = c;
    }
}

struct _div_t __cdecl div(int _Numerator, int _Denominator);
int __cdecl abs(int _X);

char *my_itoa(int n, int base, char * buf, unsigned int buf_size)
{
    struct _div_t  dv;
    char * p;

    ;
    dv.quot = n;
    dv.rem = 0;
    p = buf;
    do
    {
        if (!--buf_size)
        {
            return (*buf = 0), buf;
        }
        dv = div(dv.quot, base);
        *p++ = "0123456789abcdef"[abs(dv.rem)];
    }
    while (dv.quot);
    if (n < 0)
    {
        *p++ = 45;
    }
    *p = 0;
    reverse(buf, p);
    return buf;
}



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
    char buf[16];

    printf("%s\n", my_itoa(0, 2, buf, 16U));
    printf("%s\n", my_itoa(7, 3, buf, 16U));
    printf("%s\n", my_itoa(12346, 10, buf, 16U));
    printf("%s\n", my_itoa(-12346, 10, buf, 16U));
    printf("%s\n", my_itoa(-42, 2, buf, 16U));
    printf("%s\n", my_itoa(2147483647, 16, buf, 16U));
    printf("%s\n", my_itoa(-2147483648, 16, buf, 16U));
}


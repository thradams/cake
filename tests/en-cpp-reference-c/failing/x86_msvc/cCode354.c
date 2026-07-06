/* Cake 0.13.29 x86_msvc */

struct _iobuf {
    void * _Placeholder;
};

struct _Mbstatet {
    unsigned long _Wchar;
    unsigned short _Byte;
    unsigned short _State;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

static void memcpy(void * dest, const void * src, unsigned int n);
struct _Mbstatet state;

char *__cdecl setlocale(int _Category, char * _Locale);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl puts(char * _Buffer);
int __cdecl ___mb_cur_max_func(void);
unsigned int __cdecl c16rtomb(char * _S, unsigned short _C16, struct _Mbstatet * _Ps);

int main(void)
{
    unsigned short in[5];
    unsigned int in_sz;
    unsigned int __vm1;
    char * out;
    char * p;
    unsigned int out_sz;

    setlocale(0, "en_US.utf8");
    memcpy(in, ""z\xc3\x9f\xe6\xb0\xb4\xf0\x9f\x8d\x8c", 5);
    in_sz = 5;
    printf("Processing %zu UTF-16 code units: [ ", 5);
    {
        unsigned int n;

        n = 0;
        for (; n < 5; ++n)
        {
            printf("%#x ", in[n]);
        }
    }
    puts("]");
    __vm1 = ___mb_cur_max_func() * 5;
    out = _alloca(1 * __vm1);
    p = out;
    {
        unsigned int n;

        n = 0;
        for (; n < 5; ++n)
        {
            unsigned int rc;

            rc = c16rtomb(p, in[n], &state);
            if (rc == 4294967295)
            {
                break;
            }
            p += rc;
        }
    }
    out_sz = p - out;
    printf("into %zu UTF-8 code units: [ ", out_sz);
    {
        unsigned int x;

        x = 0;
        for (; x < out_sz; ++x)
        {
            printf("%#x ", +(unsigned char)out[x]);
        }
    }
    puts("]");
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

static void memcpy(void * dest, const void * src, unsigned int n)
{
    char *csrc;
    char *cdest;
    unsigned int i; 

    csrc = (char *)src;
    cdest = (char *)dest;
    for (i = 0; i < n; i++)
    {
       cdest[i] = csrc[i]; 
    }
}


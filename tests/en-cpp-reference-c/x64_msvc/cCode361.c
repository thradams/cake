/* Cake x64_msvc */

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

static void memcpy(void * dest, const void * src, unsigned long long n);
struct _Mbstatet state;

char *__cdecl setlocale(int _Category, char * _Locale);
static int __cdecl printf(char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl puts(char * _Buffer);
unsigned long long __cdecl mbrtoc16(unsigned short * _Pc16, char * _S, unsigned long long _N, struct _Mbstatet * _Ps);

int main(void)
{
    char in[11];
    unsigned long long in_sz;
    unsigned short out[11];
    char * p_in;
    char * end;
    unsigned short * p_out;
    unsigned long long out_sz;

    setlocale(0, "en_US.utf8");
    memcpy(in, "z\xc3\x9f\xe6\xb0\xb4\xf0\x9f\x8d\x8c", 11);
    in_sz = 11ULL;
    printf("Processing %zu UTF-8 code units: [ ", 11ULL);
    {
        unsigned long long n;

        n = 0;
        for (; n < 11ULL; ++n)
        {
            printf("%#x ", (unsigned char)in[n]);
        }
    }
    puts("]");
    p_in = in;
    end = in + 11ULL;
    p_out = out;
    {
        unsigned long long rc;

        for (; (rc = mbrtoc16(p_out, p_in, end - p_in, &state)); )
        {
            if (rc == 18446744073709551615ULL)
            {
                break;
            }
            else
            {
                if (rc == 18446744073709551614ULL)
                {
                    break;
                }
                else
                {
                    if (rc == 18446744073709551613ULL)
                    {
                        p_out += 1;
                    }
                    else
                    {
                        p_in += rc;
                        p_out += 1;
                    }
                }
            }
            ;
        }
    }
    out_sz = p_out - out + 1;
    printf("into %zu UTF-16 code units: [ ", out_sz);
    {
        unsigned long long x;

        x = 0;
        for (; x < out_sz; ++x)
        {
            printf("%#x ", out[x]);
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

    ((void)(__va_start(&_ArgList, _Format)));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

static void memcpy(void * dest, const void * src, unsigned long long n)
{
    char *csrc;
    char *cdest;
    unsigned long long i; 

    csrc = (char *)src;
    cdest = (char *)dest;
    for (i = 0; i < n; i++)
    {
       cdest[i] = csrc[i]; 
    }
}


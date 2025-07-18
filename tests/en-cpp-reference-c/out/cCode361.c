struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _Mbstatet {
    unsigned long _Wchar;
    unsigned short _Byte;
    unsigned short _State;
};

struct _iobuf {
    void * _Placeholder;
};


static void _cake_memcpy(void * dest, const void * src, unsigned long n)
{
  char *csrc = (char *)src;
  char *cdest = (char *)dest;
  for (int i=0; i<n; i++) cdest[i] = csrc[i]; 
}

struct _Mbstatet  state;
char *__cdecl setlocale(int _Category, char * _Locale);


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
int __cdecl puts(char * _Buffer);
unsigned int __cdecl mbrtoc16(unsigned short * _Pc16, char * _S, unsigned int _N, struct _Mbstatet * _Ps);

int main(void)
{
    char in[11];
    unsigned int in_sz;
    unsigned short out[11];
    char * p_in;
    char * end;
    unsigned short * p_out;
    unsigned int out_sz;

    setlocale(0, "en_US.utf8");
    _cake_memcpy(in, "z\xc3\x9f\xe6\xb0\xb4\xf0\x9f\x8d\x8c", 11);
    in_sz = 11U;
    printf("Processing %zu UTF-8 code units: [ ", 11U);
    {
        unsigned int n;
        n = 0;
        for (; n < 11U; ++n)
        printf("%#x ", (unsigned char)in[n]);
    }
    puts("]");
    p_in = in;
    end = in + 11U;
    p_out = out;
    {
        unsigned int rc;
        for (; (rc = mbrtoc16(p_out, p_in, end - p_in, &state)); )
        {
            if (rc == 4294967295U)
            {
                break;
            }
            else
            {
                if (rc == 4294967294U)
                {
                    break;
                }
                else
                {
                    if (rc == 4294967293U)
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
        unsigned int x;
        x = 0;
        for (; x < out_sz; ++x)
        printf("%#x ", out[x]);
    }
    puts("]");
}


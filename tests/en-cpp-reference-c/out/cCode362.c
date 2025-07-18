struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _Mbstatet {
    unsigned long _Wchar;
    unsigned short _Byte;
    unsigned short _State;
};


static void _cake_memcpy(void * dest, const void * src, unsigned long n)
{
  char *csrc = (char *)src;
  char *cdest = (char *)dest;
  for (int i=0; i<n; i++) cdest[i] = csrc[i]; 
}

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
void *__cdecl memset(void * _Dst, int _Val, unsigned int _Size);
unsigned int __cdecl mbrtoc32(unsigned int * _Pc32, char * _S, unsigned int _N, struct _Mbstatet * _Ps);

int main(void)
{
    char in[11];
    unsigned int in_size;
    unsigned int out[11];
    unsigned int * p_out;
    char * p_in;
    char * end;
    struct _Mbstatet  state;
    unsigned int rc;
    unsigned int out_size;

    setlocale(0, "en_US.utf8");
    _cake_memcpy(in, "z\xc3\x9f\xe6\xb0\xb4\xf0\x9f\x8d\x8c", 11);
    in_size = 11U;
    printf("Processing %zu UTF-8 code units: [ ", 11U);
    {
        unsigned int i;
        i = 0;
        for (; i < 11U; ++i)
        printf("0x%02x ", (unsigned char)in[i]);
    }
    puts("]");
    p_out = out;
    p_in = in;
    end = in + 11U;
    memset(&state, 0, 8U);
    while ((rc = mbrtoc32(p_out, p_in, end - p_in, &state)))
    {
        ;
        if (rc == 4294967295U)
        {
            break;
        }
        if (rc == 4294967294U)
        {
            break;
        }
        p_in += rc;
        ++p_out;
    }
    out_size = p_out + 1 - out;
    printf("into %zu UTF-32 code units: [ ", out_size);
    {
        unsigned int i;
        i = 0;
        for (; i < out_size; ++i)
        printf("0x%08X ", out[i]);
    }
    puts("]");
}


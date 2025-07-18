struct _iobuf {
    void * _Placeholder;
};

struct _Mbstatet {
    unsigned long _Wchar;
    unsigned short _Byte;
    unsigned short _State;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


static void _cake_memcpy(void * dest, const void * src, unsigned long n)
{
  char *csrc = (char *)src;
  char *cdest = (char *)dest;
  for (int i=0; i<n; i++) cdest[i] = csrc[i]; 
}

int _Avx2WmemEnabledWeakValue = 0;
char *__cdecl setlocale(int _Category, char * _Locale);
void *__cdecl memset(void * _Dst, int _Val, unsigned int _Size);


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
unsigned int __cdecl wcrtomb(char * _Dest, unsigned short _Source, struct _Mbstatet * _State);

int main(void)
{
    struct _Mbstatet  state;
    unsigned short in[11];
    unsigned int in_sz;
    char out[];
    char * p;
    unsigned int out_sz;

    setlocale(0, "en_US.utf8");
    memset(&state, 0, 8U);
    _cake_memcpy(in, L"z\xc3\x9f\xe6\xb0\xb4\xf0\x9f\x8d\x8c", 11);
    in_sz = 11U;
    printf("Processing %zu wchar_t units: [ ", in_sz);
    {
        unsigned int n;
        n = 0;
        for (; n < in_sz; ++n)
        printf("%#x ", (unsigned int)in[n]);
    }
    puts("]");
    p = out;
    {
        unsigned int n;
        n = 0;
        for (; n < in_sz; ++n)
        {
            int rc;

            rc = wcrtomb(p, in[n], &state);
            if (rc == -1)
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
        printf("%#x ", +(unsigned char)out[x]);
    }
    puts("]");
}


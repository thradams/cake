/* Cake 0.12.26 x86_msvc */
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


static void _cake_memcpy(void * dest, const void * src, unsigned int n)
{
  char *csrc;
  char *cdest;
  unsigned int i; 

  csrc = (char *)src;
  cdest = (char *)dest;
  for (i = 0; i < n; i++) cdest[i] = csrc[i]; 
}

__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;
char *__cdecl setlocale(int _Category, char * _Locale);
void *__cdecl memset(void * _Dst, int _Val, unsigned int _Size);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl puts(char * _Buffer);
unsigned int __cdecl mbrtowc(unsigned short * _DstCh, char * _SrcCh, unsigned int _SizeInBytes, struct _Mbstatet * _State);

int main(void)
{
    struct _Mbstatet  state;
    char in[5];
    unsigned int in_sz;
    unsigned short out[];
    char * p_in;
    char * end;
    unsigned short * p_out;
    int rc;
    unsigned int out_sz;

    setlocale(0, "en_US.utf8");
    memset(&state, 0, 8);
    _cake_memcpy(in, "z\u00df\u6c34\U0001F34C", 5);
    in_sz = 5;
    printf("Processing %zu UTF-8 code units: [ ", in_sz);
    {
        unsigned int n;

        n = 0;
        for (; n < in_sz; ++n)
        printf("%#x ", (unsigned char)in[n]);
    }
    puts("]");
    p_in = in;
    end = in + in_sz;
    p_out = out;
    while ((rc = mbrtowc(p_out, p_in, end - p_in, &state)) > 0)
    {
        p_in += rc;
        p_out += 1;
    }
    out_sz = p_out - out + 1;
    printf("into %zu wchar_t units: [ ", out_sz);
    {
        unsigned int x;

        x = 0;
        for (; x < out_sz; ++x)
        printf("%#x ", out[x]);
    }
    puts("]");
}



static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__c0__OptionsStorage;
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

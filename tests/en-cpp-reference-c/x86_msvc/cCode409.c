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
static unsigned short *__cdecl wmemchr(unsigned short * _S, unsigned short _C, unsigned int _N);
char *__cdecl setlocale(int _Category, char * _Locale);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    unsigned short str[23];
    unsigned int sz;
    unsigned short target;
    unsigned short * result;

    _cake_memcpy(str, L"\xe8\xaf\xba\xe4\xb8\x8d\xe8\xbd\xbb\xe4\xbf\xa1\xef\xbc\x8c\xe6\x95\x85\xe4\xba\xba\xe4\xb8\x8d\xe8\xb4\x9f\xe6\x88\x91\0\xe8\xaf\xba\xe4\xb8\x8d\xe8\xbd\xbb\xe8\xae\xb8\xef\xbc\x8c\xe6\x95\x85\xe6\x88\x91\xe4\xb8\x8d\xe8\xb4\x9f\xe4\xba\xba\xe3\x80\x82", 23);
    sz = 23;
    target = 35768;
    result = wmemchr(str, target, sz);
    if (result)
    {
        setlocale(0, "en_US.utf8");
        printf("Found '%lc' at position %td\n", target, result - str);
    }
}



static unsigned short *__cdecl wmemchr(unsigned short * _S, unsigned short _C, unsigned int _N)
{
    unsigned int __count;

    __count = 0;
    for (; __count < _N; ++__count)
    {
        if (_S[__count] == _C)
        {
            return (unsigned short *)&_S[__count];
        }
    }
    return 0;
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

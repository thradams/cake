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

char *__cdecl setlocale(int _Category, char * _Locale);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
unsigned int __cdecl wcstombs(char * _Dest, unsigned short * _Source, unsigned int _MaxCount);

int main(void)
{
    unsigned short src[5];
    char dst[11];
    int rtn_val;

    _cake_memcpy(src, L"z\u00df\u6c34\U0001f34c", 5);
    setlocale(0, "en_US.utf8");
    printf("wide-character string: '%ls'\n", src);
    {
        unsigned int ndx;

        ndx = 0;
        for (; ndx < 5; ++ndx)
        printf("   src[%2zu] = %#8x\n", ndx, src[ndx]);
    }
    rtn_val = wcstombs(dst, src, 11);
    printf("rtn_val = %d\n", rtn_val);
    if (rtn_val > 0)
    {
        printf("multibyte string:  '%s'\n", dst);
    }
    {
        unsigned int ndx;

        ndx = 0;
        for (; ndx < 11; ++ndx)
        printf("   dst[%2zu] = %#2x\n", ndx, (unsigned char)dst[ndx]);
    }
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

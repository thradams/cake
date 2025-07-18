struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
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
unsigned int __cdecl wcstombs(char * _Dest, unsigned short * _Source, unsigned int _MaxCount);

int main(void)
{
    unsigned short src[21];
    char dst[11];
    int rtn_val;

    _cake_memcpy(src, L"z\u00df\u6c34\U0001f34c", 21);
    setlocale(0, "en_US.utf8");
    printf("wide-character string: '%ls'\n", src);
    {
        unsigned int ndx;
        ndx = 0;
        for (; ndx < 21U; ++ndx)
        printf("   src[%2zu] = %#8x\n", ndx, src[ndx]);
    }
    rtn_val = wcstombs(dst, src, 11U);
    printf("rtn_val = %d\n", rtn_val);
    if (rtn_val > 0)
    {
        printf("multibyte string:  '%s'\n", dst);
    }
    {
        unsigned int ndx;
        ndx = 0;
        for (; ndx < 11U; ++ndx)
        printf("   dst[%2zu] = %#2x\n", ndx, (unsigned char)dst[ndx]);
    }
}


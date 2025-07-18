struct _iobuf {
    void * _Placeholder;
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

char *__cdecl setlocale(int _Category, char * _Locale);


int __cdecl __stdio_common_vsscanf(unsigned __int64 _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

static unsigned __int64 __ck__OptionsStorage1;
inline unsigned __int64 *__cdecl __local_stdio_scanf_options(void)
{
    return &__ck__OptionsStorage1;
}
inline int __cdecl _vsscanf_l(char * _Buffer, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vsscanf((*__local_stdio_scanf_options()), _Buffer, 4294967295U, _Format, _Locale, _ArgList);
}
inline int __cdecl sscanf(char * _Buffer, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vsscanf_l(_Buffer, _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}


int __cdecl __stdio_common_vfprintf(unsigned __int64 _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

static unsigned __int64 __ck__OptionsStorage2;
inline unsigned __int64 *__cdecl __local_stdio_printf_options(void)
{
    return &__ck__OptionsStorage2;
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
    int i;
    int j;
    float x;
    float y;
    char str1[10];
    char str2[4];
    unsigned short warr[2];
    char input[40];
    int ret;

    setlocale(0, "en_US.utf8");
    _cake_memcpy(input, "25 54.32E-1 Thompson 56789 0123 56\xc3\x9f\xe6\xb0\xb4", 40);
    ret = sscanf(input, "%d%f%9s%2d%f%*d %3[0-9]%2lc", &i, &x, str1, &j, &y, str2, warr);
    printf("Converted %d fields:\n""i = %d\n""x = %f\n""str1 = %s\n""j = %d\n""y = %f\n""str2 = %s\n""warr[0] = U+%x\n""warr[1] = U+%x\n", ret, i, x, str1, j, y, str2, warr[0], warr[1]);
}


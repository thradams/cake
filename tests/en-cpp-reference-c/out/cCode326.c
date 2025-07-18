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

int __cdecl puts(char * _Buffer);
void *__cdecl memmove(void * _Dst, void * _Src, unsigned int _Size);
void *__cdecl malloc(unsigned int _Size);


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

int main(void)
{
    char str[11];
    int * p;
    int arr[3];
    double d;
    long long n;

    _cake_memcpy(str, "1234567890", 11);
    puts(str);
    memmove(str + 4, str + 3, 3);
    puts(str);
    p = malloc(12);
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    memmove(p, arr, 12);
    d = 0.100000;
    memmove(&n, &d, 8U);
    printf("%a is %""llx"" as an int64_t\n", d, n);
}


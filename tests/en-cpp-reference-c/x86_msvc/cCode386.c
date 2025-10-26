/* Cake 0.12.26 x86_msvc */
struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
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
unsigned short __cdecl towctrans(unsigned short c, unsigned short value);
unsigned short __cdecl wctrans(char * name);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    unsigned short kana[5];
    unsigned int sz;
    unsigned short hira[];

    setlocale(0, "ja_JP.UTF-8");
    _cake_memcpy(kana, L"\xe3\x83\x92\xe3\x83\xa9\xe3\x82\xac\xe3\x83\x8a", 5);
    sz = 5;
    {
        unsigned int n;

        n = 0;
        for (; n < sz; ++n)
        hira[n] = towctrans(kana[n], wctrans("tojhira"));
    }
    printf("katakana characters %ls are %ls in hiragana\n", kana, hira);
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

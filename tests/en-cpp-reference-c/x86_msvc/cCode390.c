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
unsigned short *__cdecl wcschr(unsigned short * _Str, unsigned short _Ch);
char *__cdecl setlocale(int _Category, char * _Locale);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl puts(char * _Buffer);

int main(void)
{
    unsigned short arr[12];
    unsigned short * cat;
    unsigned short * dog;

    _cake_memcpy(arr, L"\xe7\x99\xbd\xe7\x8c\xab \xe9\xbb\x92\xe7\x8c\xab \xd0\xba\xd0\xbe\xd1\x88\xd0\xba\xd0\xb8", 12);
    cat = wcschr(arr, 29483);
    dog = wcschr(arr, 29356);
    setlocale(0, "en_US.utf8");
    if (cat)
    {
        printf("The character \xe7\x8c\xab found at position %td\n", cat - arr);
    }
    else
    {
        puts("The character \xe7\x8c\xab not found");
    }
    if (dog)
    {
        printf("The character \xe7\x8a\xac found at position %td\n", dog - arr);
    }
    else
    {
        puts("The character \xe7\x8a\xac not found");
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

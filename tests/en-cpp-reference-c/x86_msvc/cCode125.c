/* Cake 0.12.26 x86_msvc */
struct __tag0 {
    int arr[2];
};

struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    int i;
    int j;
    int k;
    char c;
    char * p;
    char ** cpp;
    int arr1[2];
    int arr2[2];
    struct __tag0  sam1;
    struct __tag0  sam2;

    i = 1;
    j = 2;
    k = 3;
    i = j = k;
    printf("%d %d %d\n", i, j, k);
    c = 65;
    p = &c;
    cpp = &p;
    *cpp = &c;
    printf("%c \n", **cpp);
    cpp = 0;
    arr1[0] = 1;
    arr1[1] = 2;
    arr2[0] = 3;
    arr2[1] = 4;
    printf("arr1[0]=%d arr1[1]=%d arr2[0]=%d arr2[1]=%d\n", arr1[0], arr1[1], arr2[0], arr2[1]);
    sam1.arr[0] = 5;
    sam1.arr[1] = 6;
    sam2.arr[0] = 7;
    sam2.arr[1] = 8;
    sam1 = sam2;
    printf("%d %d \n", sam1.arr[0], sam1.arr[1]);
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

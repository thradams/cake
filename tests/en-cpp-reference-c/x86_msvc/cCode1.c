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


int compare_ints(void * a, void * b)
{
    int arg1;
    int arg2;

    arg1 = *(int *)a;
    arg2 = *(int *)b;
    if (arg1 < arg2)
    {
        return -1;
    }
    if (arg1 > arg2)
    {
        return 1;
    }
    return 0;
}


void __cdecl qsort(void * _Base, unsigned int _NumOfElements, unsigned int _SizeOfElements, int (__cdecl * _CompareFunction)(void *, void *));
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    int ints[7];
    int size;

    ints[0] = -2;
    ints[1] = 99;
    ints[2] = 0;
    ints[3] = -743;
    ints[4] = 2;
    ints[5] = -2147483648;
    ints[6] = 4;
    size = 7;
    qsort(ints, size, 4, compare_ints);
    {
        int i;

        i = 0;
        for (; i < size; i++)
        {
            printf("%d ", ints[i]);
        }
    }
    printf("\n");
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

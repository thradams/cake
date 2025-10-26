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


static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void print_storage_info(int * next, int * prev, int ints)
{
    if (next)
    {
        printf("%s location: %p. Size: %d ints (%ld bytes).\n", (next != prev ? "New" : "Old"), (void *)next, ints, ints * 4);
    }
    else
    {
        printf("Allocation failed.\n");
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
void *__cdecl malloc(unsigned int _Size);
void *__cdecl memcpy(void * _Dst, void * _Src, unsigned int _Size);
void *__cdecl realloc(void * _Block, unsigned int _Size);
void __cdecl free(void * _Block);

int main(void)
{
    int pattern[8];
    int pattern_size;
    int * next;
    int * prev;
    int realloc_size[6];

    pattern[0] = 1;
    pattern[1] = 2;
    pattern[2] = 3;
    pattern[3] = 4;
    pattern[4] = 5;
    pattern[5] = 6;
    pattern[6] = 7;
    pattern[7] = 8;
    pattern_size = 8;
    next = 0;
    prev = 0;
    if ((next = (int *)malloc(32)))
    {
        memcpy(next, pattern, 32);
        print_storage_info(next, prev, 8);
    }
    else
    {
        return 1;
    }
    realloc_size[0] = 10;
    realloc_size[1] = 12;
    realloc_size[2] = 512;
    realloc_size[3] = 32768;
    realloc_size[4] = 65536;
    realloc_size[5] = 32768;
    {
        int i;

        i = 0;
        for (; i != 6; ++i)
        {
            if ((next = (int *)realloc(prev = next, realloc_size[i] * 4)))
            {
                print_storage_info(next, prev, realloc_size[i]);
                ;
            }
            else
            {
                free(prev);
                return 1;
            }
        }
    }
    free(next);
    return 0;
}



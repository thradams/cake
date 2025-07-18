struct _iobuf {
    void * _Placeholder;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};




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

void print_storage_info(int * next, int * prev, int ints)
{
    if (next)
    {
        printf("%s location: %p. Size: %d ints (%ld bytes).\n", (next != prev ? "New" : "Old"), (void *)next, ints, ints * 4U);
    }
    else
    {
        printf("Allocation failed.\n");
    }
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
    pattern_size = 8U;
    next = 0U;
    prev = 0U;
    if ((next = (int *)malloc(32)))
    {
        memcpy(next, pattern, 32U);
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
        for (; i != 6U; ++i)
        {
            if ((next = (int *)realloc(prev = next, realloc_size[i] * 4U)))
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


/* Cake 0.13.29 x86_msvc */

struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};
unsigned char image_data[176] = {137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,8,0,0,0,5,8,6,0,0,0,120,145,173,85,0,0,0,1,115,82,71,66,0,174,206,28,233,0,0,0,4,103,65,77,65,0,0,177,143,11,252,97,5,0,0,0,9,112,72,89,115,0,0,22,37,0,0,22,37,1,73,82,36,240,0,0,0,69,73,68,65,84,24,87,93,202,193,9,0,48,8,67,81,143,142,224,209,81,220,206,49,28,195,241,210,42,88,176,31,114,8,60,194,45,51,97,102,136,136,186,171,6,34,2,34,234,169,234,130,13,152,249,129,90,253,65,13,220,125,129,65,15,76,63,4,128,3,56,210,100,63,37,42,62,117,0,0,0,0,73,69,78,68,174,66,96,130};

static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl puts(char * _Buffer);

void dump(unsigned char arr[], unsigned int size)
{
    {
        unsigned int i;

        i = 0;
        for (; i != size; ++i)
        {
            printf("%02X%c", arr[i], (i + 1) % 16 ? 32 : 10);
        }
    }
    puts("");
}

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &_OptionsStorage;
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

int main()
{
    puts("image_data[]:");
    dump(image_data, 176);
    puts("message[]:");
}

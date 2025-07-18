struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
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
inline int __cdecl vfprintf(struct _iobuf * _Stream, char * _Format, char * _ArgList)
{
    return _vfprintf_l(_Stream, _Format, 0U, _ArgList);
}
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int my_printf(char * fmt, ...)
{
    char * vl;
    int ret;

    ((void)(vl = (char *)(&(fmt)) + 4U));
    ret = vfprintf((__acrt_iob_func(1)), fmt, vl);
    ((void)(vl = 0U));
    return ret;
}

int main(void)
{
    my_printf("Rounding:\t%f %.0f %.32f\n", 1.500000, 1.500000, 1.300000);
    my_printf("Padding:\t%05.2f %.2f %5.2f\n", 1.500000, 1.500000, 1.500000);
    my_printf("Scientific:\t%E %e\n", 1.500000, 1.500000);
    my_printf("Hexadecimal:\t%a %A\n", 1.500000, 1.500000);
}


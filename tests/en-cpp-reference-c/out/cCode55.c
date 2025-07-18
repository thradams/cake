struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


int _Avx2WmemEnabledWeakValue = 0;


int __cdecl __stdio_common_vswscanf(unsigned __int64 _Options, unsigned short * _Buffer, unsigned int _BufferCount, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

static unsigned __int64 __ck__OptionsStorage1;
inline unsigned __int64 *__cdecl __local_stdio_scanf_options(void)
{
    return &__ck__OptionsStorage1;
}
inline int __cdecl _vswscanf_l(unsigned short * _Buffer, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vswscanf((*__local_stdio_scanf_options()), _Buffer, 4294967295U, _Format, _Locale, _ArgList);
}
inline int __cdecl swscanf(unsigned short * _Buffer, unsigned short * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vswscanf_l(_Buffer, _Format, 0U, _ArgList);
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
inline int __cdecl fprintf(struct _iobuf * _Stream, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfprintf_l(_Stream, _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);


int __cdecl __stdio_common_vfwprintf(unsigned __int64 _Options, struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
inline int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfwprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}
inline int __cdecl wprintf(unsigned short * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfwprintf_l((__acrt_iob_func(1)), _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}
struct _iobuf *__cdecl tmpfile(void);

inline int __cdecl fwprintf(struct _iobuf * _Stream, unsigned short * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfwprintf_l(_Stream, _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}
int __cdecl fclose(struct _iobuf * _Stream);
void __cdecl rewind(struct _iobuf * _Stream);


int __cdecl __stdio_common_vfwscanf(unsigned __int64 _Options, struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
inline int __cdecl _vfwscanf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfwscanf((*__local_stdio_scanf_options()), _Stream, _Format, _Locale, _ArgList);
}
inline int __cdecl fwscanf(struct _iobuf * _Stream, unsigned short * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4U));
    _Result = _vfwscanf_l(_Stream, _Format, 0U, _ArgList);
    ((void)(_ArgList = 0U));
    return _Result;
}

int main(void)
{
    unsigned short state[64];
    unsigned short capital[64];
    unsigned int population;
    int elevation;
    int age;
    float pi;
    unsigned short * input;
    struct _iobuf * fp;

    population = 0;
    elevation = 0;
    age = 0;
    pi = 0;
    input = L"California 170 3.141592";
    if (swscanf(input, L"%ls%d%f", state, &age, &pi) != 3)
    {
        fprintf((__acrt_iob_func(2)), "Error reading input.\n");
        return 2;
    }
    wprintf(L"State: %ls\nAge  : %d years\nPi   : %.5f\n\n", state, age, pi);
    fp = tmpfile();
    if (fp)
    {
        if (!fwprintf(fp, L"Mississippi Jackson 420000 807"))
        {
            fprintf((__acrt_iob_func(2)), "Error writing to file.\n");
            fclose(fp);
            return 3;
        }
        rewind(fp);
        fwscanf(fp, L"%ls%ls%u%d", state, capital, &population, &elevation);
        wprintf(L"State  : %ls\nCapital: %ls\nJackson population (in 2020): %u\n"L"Highest elevation: %dft\n", state, capital, population, elevation);
        fclose(fp);
    }
}


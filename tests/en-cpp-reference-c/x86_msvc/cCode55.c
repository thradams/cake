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


__declspec(selectany) int _Avx2WmemEnabledWeakValue = 0;
static int __cdecl swscanf(unsigned short * _Buffer, unsigned short * _Format, ...);
static int __cdecl _vswscanf_l(unsigned short * _Buffer, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vswscanf(unsigned long long _Options, unsigned short * _Buffer, unsigned int _BufferCount, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_scanf_options(void);
static unsigned long long __c0__OptionsStorage;
static int __cdecl fprintf(struct _iobuf * _Stream, char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c1__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
static int __cdecl wprintf(unsigned short * _Format, ...);
static int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfwprintf(unsigned long long _Options, struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
struct _iobuf *__cdecl tmpfile(void);
static int __cdecl fwprintf(struct _iobuf * _Stream, unsigned short * _Format, ...);
int __cdecl fclose(struct _iobuf * _Stream);
void __cdecl rewind(struct _iobuf * _Stream);
static int __cdecl fwscanf(struct _iobuf * _Stream, unsigned short * _Format, ...);
static int __cdecl _vfwscanf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfwscanf(unsigned long long _Options, struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

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
        wprintf(L"State  : %ls\nCapital: %ls\nJackson population (in 2020): %u\nHighest elevation: %dft\n", state, capital, population, elevation);
        fclose(fp);
    }
}



static unsigned long long *__cdecl __local_stdio_scanf_options(void)
{
    return &__c0__OptionsStorage;
}

static int __cdecl _vswscanf_l(unsigned short * _Buffer, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vswscanf((*__local_stdio_scanf_options()), _Buffer, 4294967295, _Format, _Locale, _ArgList);
}

static int __cdecl swscanf(unsigned short * _Buffer, unsigned short * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vswscanf_l(_Buffer, _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__c1__OptionsStorage;
}

static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl fprintf(struct _iobuf * _Stream, char * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vfprintf_l(_Stream, _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

static int __cdecl _vfwprintf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfwprintf((*__local_stdio_printf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl wprintf(unsigned short * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vfwprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

static int __cdecl fwprintf(struct _iobuf * _Stream, unsigned short * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vfwprintf_l(_Stream, _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

static int __cdecl _vfwscanf_l(struct _iobuf * _Stream, unsigned short * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfwscanf((*__local_stdio_scanf_options()), _Stream, _Format, _Locale, _ArgList);
}

static int __cdecl fwscanf(struct _iobuf * _Stream, unsigned short * _Format, ...)
{
    int _Result;
    char * _ArgList;

    ((void)(_ArgList = (char *)(&(_Format)) + 4));
    _Result = _vfwscanf_l(_Stream, _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

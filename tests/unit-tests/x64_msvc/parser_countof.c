/* Cake x64_msvc */

struct _iobuf {
    void * _Placeholder;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

static void test_vm_pointer(int n);
static void check(char * name, int ok);
static int __cdecl printf(char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
static int failures = 0;

static void test_vm_parameter(void);
static void check_param_countof(int arr[1]);

int main(void)
{
    test_vm_pointer(8);
    test_vm_parameter();
    if (failures == 0)
    {
        printf("\nAll tests passed.\n");
    }
    else
    {
        printf("\n%d test(s) FAILED.\n", failures);
    }
    return failures != 0;
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

    ((void)(__va_start(&_ArgList, _Format)));
    _Result = _vfprintf_l((__acrt_iob_func(1)), _Format, 0, _ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}

static void check(char * name, int ok)
{
    if (ok)
    {
        printf("PASS  %s\n", name);
    }
    else
    {
        printf("FAIL  %s\n", name);
        failures++;
    }
}

static void test_vm_pointer(int n)
{
    unsigned long long __a;
    int (* p)[1];

    __a = n;
    p = 0;
    check("VM pointer-to-array countof",  == (unsigned long long)n);
}

static void check_param_countof(int arr[1])
{
    check("VM param inner dim", 1);
}

static void test_vm_parameter(void)
{
    int data[6][4];

    check_param_countof(data);
}

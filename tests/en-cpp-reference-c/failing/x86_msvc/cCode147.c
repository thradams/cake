/* Cake 0.13.29 x86_msvc */

struct spaceship {
    char * model;
    int max_speed;
};

struct car {
    char * make;
    int year;
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
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    struct car c;
    struct spaceship ship;
    struct spaceship * pship;
    char * pmake;

    c.make = "Nash";
    c.year = 1923;
    printf("Car: %d %s\n", c.year, c.make);
    ship.model = "T-65 X-wing starfighter";
    ship.max_speed = 1050;
    pship = &ship;
    printf("Spaceship: %s. Max speed: %d km/h\n\n", ship.model, ship.max_speed);
    printf("Offset of char a = %zu\nOffset of double b = %zu\nOffset of char c = %zu\nSize of struct A = %zu\n\n", 0, 8, 16, 24);
    printf("Offset of char a = %zu\nOffset of char b = %zu\nOffset of double c = %zu\nSize of struct B = %zu\n\n", 0, 1, 8, 16);
    pmake = (char *)pship;
    pship = (struct spaceship *)pmake;
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

/* Cake x64_msvc */

struct point {
    double x;
    double y;
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

static int _obj[2] = {2,4};
int * p = _obj;

static float _obj2[3] = {1.0,10.0,100.0};
float * pc = _obj2;

void drawline1(struct point from, struct point to);
void drawline2(struct point * from, struct point * to);

int main(void)
{
    int n;
    int * p;
    int __a[2];
    struct point __b;
    struct point __c;
    struct point __d;
    struct point __e;

    n = 2;
    p = &n;
    __a[0] = *p;
    __a[1] = 0;
    p = __a;
    __b.x = 1;
    __b.y = 1;
    __c.x = 3;
    __c.y = 4;
    drawline1(__b, __c);
    __d.x = 1;
    __d.y = 1;
    __e.x = 3;
    __e.y = 4;
    drawline2(&__d, &__e);
}

static int __cdecl printf(char * _Format, ...);
void __cdecl __va_start(char **, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long _OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void drawline1(struct point from, struct point to)
{
    printf("drawline1: `from` @ %p {%.2f, %.2f}, `to` @ %p {%.2f, %.2f}\n", (void *)&from, from.x, from.y, (void *)&to, to.x, to.y);
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

void drawline2(struct point * from, struct point * to)
{
    printf("drawline2: `from` @ %p {%.2f, %.2f}, `to` @ %p {%.2f, %.2f}\n", (void *)from, from->x, from->y, (void *)to, to->x, to->y);
}

/* Cake 0.12.26 x86_msvc */
int __v0[2] = {2,4};
float __v1[3] = {1.0,10.0,100.0};
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


int * p = __v0;
float * pc = __v1;
void drawline1(struct point  from, struct point  to);
void drawline2(struct point * from, struct point * to);

int main(void)
{
    int n;
    int * p;
    int __v0[2];
    struct point  __v1;
    struct point  __v2;
    struct point  __v3;
    struct point  __v4;

    n = 2;
    p = &n;
    __v0[0] = *p;
    __v0[1] = 0;
    p = __v0;
    __v1.x = 1;
    __v1.y = 1;
    __v2.x = 3;
    __v2.y = 4;
    drawline1(__v1, __v2);
    __v3.x = 1;
    __v3.y = 1;
    __v4.x = 3;
    __v4.y = 4;
    drawline2(&__v3, &__v4);
}


static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void drawline1(struct point  from, struct point  to)
{
    printf("drawline1: `from` @ %p {%.2f, %.2f}, `to` @ %p {%.2f, %.2f}\n", (void *)&from, from.x, from.y, (void *)&to, to.x, to.y);
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
void drawline2(struct point * from, struct point * to)
{
    printf("drawline2: `from` @ %p {%.2f, %.2f}, `to` @ %p {%.2f, %.2f}\n", (void *)from, from->x, from->y, (void *)to, to->x, to->y);
}



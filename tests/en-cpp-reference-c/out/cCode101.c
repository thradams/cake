int __cmp_lt_0[2] = {2, 4};
float __cmp_lt_1[3] = {1, 1, 1};
struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct point {
    double x;
    double y;
};

struct _iobuf {
    void * _Placeholder;
};


int * p = __cmp_lt_0;
float * pc = __cmp_lt_1;
void drawline1(struct point  from, struct point  to);
void drawline2(struct point * from, struct point * to);

int main(void)
{
    int n;
    int * p;
    int __cmp_lt_2[2];
    struct point  __cmp_lt_3;
    struct point  __cmp_lt_4;
    struct point  __cmp_lt_5;
    struct point  __cmp_lt_6;

    n = 2;
    p = &n;
    __cmp_lt_2[0] = *p;
    __cmp_lt_2[1] = 0;
    p = __cmp_lt_2;
    __cmp_lt_3.x = 1;
    __cmp_lt_3.y = 1;
    __cmp_lt_4.x = 3;
    __cmp_lt_4.y = 4;
    drawline1(__cmp_lt_3, __cmp_lt_4);
    __cmp_lt_5.x = 1;
    __cmp_lt_5.y = 1;
    __cmp_lt_6.x = 3;
    __cmp_lt_6.y = 4;
    drawline2(&__cmp_lt_5, &__cmp_lt_6);
}



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

void drawline1(struct point  from, struct point  to)
{
    printf("drawline1: `from` @ %p {%.2f, %.2f}, `to` @ %p {%.2f, %.2f}\n", (void *)&from, from.x, from.y, (void *)&to, to.x, to.y);
}

void drawline2(struct point * from, struct point * to)
{
    printf("drawline2: `from` @ %p {%.2f, %.2f}, `to` @ %p {%.2f, %.2f}\n", (void *)from, from->x, from->y, (void *)to, to->x, to->y);
}


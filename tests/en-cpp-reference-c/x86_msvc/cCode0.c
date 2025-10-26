/* Cake 0.12.26 x86_msvc */
struct _iobuf {
    void * _Placeholder;
};

struct data {
    int nr;
    char * value;
};

struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


struct data dat[4] = {1,"Foo",2,"Bar",3,"Hello",4,"World"};
int data_cmp(void * lhs, void * rhs)
{
    struct data * l;
    struct data * r;

    l = lhs;
    r = rhs;
    if (l->nr < r->nr)
    {
        return -1;
    }
    else
    {
        if (l->nr > r->nr)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}


void *__cdecl bsearch(void * _Key, void * _Base, unsigned int _NumOfElements, unsigned int _SizeOfElements, int (__cdecl * _CompareFunction)(void *, void *));
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    struct data  key;
    struct data * res;

    key.nr = 3;
    key.value = 0;
    res = bsearch(&key, dat, 4, 8, data_cmp);
    if (res)
    {
        printf("No %d: %s\n", res->nr, res->value);
    }
    else
    {
        printf("No %d not found\n", key.nr);
    }
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

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


static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

int main(void)
{
    int i;

    i = 2;
    /*switch*/
    {
        int __v0 = i;
        if (__v0 == 1) goto __L1; /*case 1*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 3) goto __L3; /*case 3*/
        if (__v0 == 4) goto __L4; /*case 4*/
        if (__v0 == 5) goto __L5; /*case 5*/
        if (__v0 == 6) goto __L6; /*case 6*/
        goto __L0;

        {
            __L1: /*case 1*/ 
            printf("1");
            __L2: /*case 2*/ 
            printf("2");
            __L3: /*case 3*/ 
            printf("3");
            __L4: /*case 4*/ 
            __L5: /*case 5*/ 
            printf("45");
            goto __L0; /* break */

            __L6: /*case 6*/ 
            printf("6");
        }
        __L0:;
    }
    printf("\n");
    {
        int j;

        j = 0;
        for (; j < 2; j++)
        {
            int k;

            k = 0;
            for (; k < 5; k++)
            printf("%d%d ", j, k);
        }
    }
    printf("\n");
    {
        int j;

        j = 0;
        for (; j < 2; j++)
        {
            {
                int k;

                k = 0;
                for (; k < 5; k++)
                {
                    if (k == 2)
                    {
                        break;
                    }
                    printf("%d%d ", j, k);
                }
            }
        }
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

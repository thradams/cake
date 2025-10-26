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


int __cdecl isspace(int _C);
int __cdecl getc(struct _iobuf * _Stream);
int __cdecl isdigit(int _C);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl ungetc(int _Character, struct _iobuf * _Stream);

void demo_scanf(char * fmt, struct _iobuf * s)
{
    while (*fmt != 0)
    {
        if (*fmt == 37)
        {
            int c;

            /*switch*/
            {
                char __v0 = *++fmt;
                if (__v0 == 117) goto __L2; /*case 117*/
                if (__v0 == 99) goto __L3; /*case 99*/
                goto __L1;

                {
                    unsigned int num;

                    __L2: /*case 117*/ 
                    while (isspace(c = getc(s)))
                    {
                    }
                    num = 0;
                    while (isdigit(c))
                    {
                        num = num * 10 + c - 48;
                        c = getc(s);
                    }
                    printf("%%u scanned %u\n", num);
                    ungetc(c, s);
                    goto __L1; /* break */

                    __L3: /*case 99*/ 
                    c = getc(s);
                    printf("%%c scanned '%c'\n", c);
                    goto __L1; /* break */

                }
                __L1:;
            }
        }
        else
        {
            ++fmt;
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
struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);
int __cdecl fputs(char * _Buffer, struct _iobuf * _Stream);
void __cdecl rewind(struct _iobuf * _Stream);
int __cdecl fclose(struct _iobuf * _Stream);

int main(void)
{
    struct _iobuf * f;

    f = fopen("input.txt", "w+");
    if (f != 0)
    {
        fputs("123x", f);
        rewind(f);
        demo_scanf("%u%c", f);
        fclose(f);
    }
    return 0;
}



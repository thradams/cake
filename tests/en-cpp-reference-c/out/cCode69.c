struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


int __cdecl isspace(int _C);
int __cdecl getc(struct _iobuf * _Stream);
int __cdecl isdigit(int _C);


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
                register char _R0 = *++fmt;
                if (_R0 == 117) goto _CKL2; /*case 117*/
                if (_R0 == 99) goto _CKL3; /*case 99*/
                goto _CKL1;

                {
                    unsigned int num;

                    /*case 117*/ _CKL2:
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
                    /*break*/ goto _CKL1;

                    /*case 99*/ _CKL3:
                    c = getc(s);
                    printf("%%c scanned '%c'\n", c);
                    /*break*/ goto _CKL1;

                }
                _CKL1:;
            }
        }
        else
        {
            ++fmt;
        }
    }
}

struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);
int __cdecl fputs(char * _Buffer, struct _iobuf * _Stream);
void __cdecl rewind(struct _iobuf * _Stream);
int __cdecl fclose(struct _iobuf * _Stream);

int main(void)
{
    struct _iobuf * f;

    f = fopen("input.txt", "w+");
    if (f != 0U)
    {
        fputs("123x", f);
        rewind(f);
        demo_scanf("%u%c", f);
        fclose(f);
    }
    return 0;
}


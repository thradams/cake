/* 
   The objective of this output is to generate a simple yet compatible 
   version of C89, which can serve as an intermediate language for backends.
*/

/*forward declarations*/
struct _iobuf;
struct __crt_locale_data;
struct __crt_locale_pointers;
struct __crt_multibyte_data;


/*declarations*/
struct _iobuf
{
    void * _Placeholder;
};

struct __crt_locale_data;
struct __crt_locale_pointers
{
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct __crt_multibyte_data;




int __stdio_common_vfprintf(unsigned __int64 _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

inline unsigned __int64 *__local_stdio_printf_options(void)
{
    static unsigned __int64 _OptionsStorage;
    return &_OptionsStorage;
}
inline int _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    return __stdio_common_vfprintf(( *__local_stdio_printf_options()),_Stream,_Format,_Locale,_ArgList);
}
struct _iobuf *__acrt_iob_func(unsigned int _Ix);
inline int printf(char * _Format, ...)
{
    int _Result;
    char * _ArgList;
    ((void)(_ArgList = (char *)(&(_Format)) + ((sizeof (_Format) + sizeof (int) - 1) &  ~(sizeof (int) - 1))));
    _Result = _vfprintf_l((__acrt_iob_func(1)),_Format,((void *)0),_ArgList);
    ((void)(_ArgList = (char *)0));
    return _Result;
}
int isalnum(int _C);
char *setlocale(int _Category, char * _Locale);


int main(void)
{
    unsigned char c = 223;
    printf("isalnum('\\xdf') in default C locale returned %d\n", ! !isalnum(c));
    if (setlocale(2,"de_DE.iso88591"))
    {
        printf("isalnum('\\xdf') in ISO-8859-1 locale returned %d\n", ! !isalnum(c));
    }
}


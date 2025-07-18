struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


unsigned int __cdecl strlen(char * _Str);
int __cdecl mblen(char * _Ch, unsigned int _MaxCount);
void __cdecl perror(char * _ErrorMessage);

unsigned int strlen_mb(char * ptr)
{
    unsigned int result;
    char * end;

    result = 0;
    end = ptr + strlen(ptr);
    mblen(0U, 0);
    while (ptr < end)
    {
        int next;

        next = mblen(ptr, end - ptr);
        if (next == -1)
        {
            perror("strlen_mb");
            break;
        }
        ptr += next;
        ++result;
    }
    return result;
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

void dump_bytes(char * str)
{
    {
        char * end;
        end = str + strlen(str);
        for (; str != end; ++str)
        printf("%02X ", (unsigned char)str[0]);
    }
    printf("\n");
}

char *__cdecl setlocale(int _Category, char * _Locale);

int main(void)
{
    char * str;

    setlocale(0, "en_US.utf8");
    str = "z\u00df\u6c34\U0001f34c";
    printf("The string \"%s\" consists of %zu characters, but %zu bytes: ", str, strlen_mb(str), strlen(str));
    dump_bytes(str);
}


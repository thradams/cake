struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


int _Avx2WmemEnabledWeakValue = 0;
int __cdecl puts(char * _Buffer);

void show_orientation(int n)
{
    n < 0 ? puts("\tnarrow orientation") : n > 0 ? puts("\twide orientation") : puts("\tno orientation");
}

int __cdecl fgetc(struct _iobuf * _Stream);


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
unsigned short __cdecl fgetwc(struct _iobuf * _Stream);

void try_read(struct _iobuf * fp)
{
    int c;
    unsigned short wc;

    c = fgetc(fp);
    if (c == -1)
    {
        puts("\tnarrow character read failed");
    }
    else
    {
        printf("\tnarrow character read '%c'\n", c);
    }
    wc = fgetwc(fp);
    if (wc == 65535)
    {
        puts("\twide character read failed");
    }
    else
    {
        printf("\twide character read '%lc'\n", wc);
    }
}

struct _iobuf *__cdecl fopen(char * _FileName, char * _Mode);
void __cdecl perror(char * _ErrorMessage);

inline int __cdecl fwide(struct _iobuf * _F, int _M)
{
    (void)_F;
    return (_M);
}
struct _iobuf *__cdecl freopen(char * _FileName, char * _Mode, struct _iobuf * _Stream);
int __cdecl fclose(struct _iobuf * _Stream);

int main(void)
{
    struct _iobuf * fp;

    fp = fopen("main.cpp", "r");
    if (!fp)
    {
        perror("fopen() failed");
        return 1;
    }
    puts("1) A newly opened stream has no orientation.");
    show_orientation(fwide(fp, 0));
    puts("2) Establish byte orientation.");
    show_orientation(fwide(fp, -1));
    try_read(fp);
    puts("3) Only freopen() can reset stream orientation.");
    if (freopen("main.cpp", "r", fp) == 0U)
    {
        perror("freopen() failed");
        return 1;
    }
    puts("4) A reopened stream has no orientation.");
    show_orientation(fwide(fp, 0));
    puts("5) Establish wide orientation.");
    show_orientation(fwide(fp, 1));
    try_read(fp);
    fclose(fp);
}


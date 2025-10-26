/* Cake 0.12.26 x86_msvc */
struct __crt_multibyte_data;

struct __crt_locale_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


void *__stdcall GetStdHandle(unsigned long nStdHandle);
int __stdcall GetConsoleMode(void * hConsoleHandle, unsigned long * lpMode);
int __stdcall SetConsoleMode(void * hConsoleHandle, unsigned long dwMode);
int __stdcall SetConsoleOutputCP(unsigned int wCodePageID);

unsigned char enable_vt_mode(void)
{
    unsigned long mode;
    void * h_out;

    mode = 0;
    h_out = GetStdHandle(4294967285L);
    if (h_out != ((void *)-1L) && GetConsoleMode(h_out, &mode) != 0 && SetConsoleMode(h_out, mode |= 4) != 0 && SetConsoleOutputCP(65001) != 0)
    {
        return 1;
    }
    return 0;
}


int __cdecl _kbhit(void);

int c_kbhit(void)
{
    return _kbhit();
}


int __cdecl _getch(void);

int c_getch(void)
{
    return _getch();
}


int __cdecl puts(char * _Buffer);
int __cdecl fflush(struct _iobuf * _Stream);
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);

void c_clrscr()
{
    puts("\x1b[2J\x1b[1;1H");
    puts("\x1b[3J");
    fflush((__acrt_iob_func(1)));
}


static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;

void c_gotoxy(int x, int y)
{
    printf("\x1b[%d;%dH", y, x);
    fflush((__acrt_iob_func(1)));
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

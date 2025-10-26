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


static void _cake_memcpy(void * dest, const void * src, unsigned int n)
{
  char *csrc;
  char *cdest;
  unsigned int i; 

  csrc = (char *)src;
  cdest = (char *)dest;
  for (i = 0; i < n; i++) cdest[i] = csrc[i]; 
}

void *__cdecl memccpy(void * _Dst, void * _Src, int _Val, unsigned int _Size);
static int __cdecl printf(char * _Format, ...);
static int __cdecl _vfprintf_l(struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
int __cdecl __stdio_common_vfprintf(unsigned long long _Options, struct _iobuf * _Stream, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
struct _iobuf *__cdecl __acrt_iob_func(unsigned int _Ix);
int __cdecl putchar(int _Character);
int __cdecl isprint(int _C);
int __cdecl puts(char * _Buffer);

int main(void)
{
    char src[26];
    char terminal[5];
    char dest[26];
    char alt;
    char *star_distance[5];
    char names_only[64];
    char * first;
    char * last;

    _cake_memcpy(src, "Stars: Altair, Sun, Vega.", 26);
    terminal[0] = 58;
    terminal[1] = 32;
    terminal[2] = 44;
    terminal[3] = 46;
    terminal[4] = 33;
    alt = 64;
    {
        unsigned int i;

        i = 0;
        for (; i != 5; ++i)
        {
            void * to;

            to = memccpy(dest, src, terminal[i], 26);
            printf("Terminal '%c' (%s):\t\"", terminal[i], to ? "found" : "absent");
            to = to ? to : dest + 26;
            {
                char * from;

                from = dest;
                for (; from != to; ++from)
                putchar(isprint(*from) ? *from : 64);
            }
            puts("\"");
        }
    }
    puts("\nSeparate star names from distances (ly):");
    star_distance[0] = "Arcturus : 37";
    star_distance[1] = "Vega : 25";
    star_distance[2] = "Capella : 43";
    star_distance[3] = "Rigel : 860";
    star_distance[4] = "Procyon : 11";
    first = names_only;
    last = names_only + 64;
    {
        unsigned int t;

        t = 0;
        for (; t != 5; ++t)
        {
            if (first)
            {
                first = memccpy(first, star_distance[t], 32, last - first);
            }
            else
            {
                break;
            }
        }
    }
    if (first)
    {
        *first = 0;
        puts(names_only);
    }
    else
    {
        puts("Buffer is too small.");
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

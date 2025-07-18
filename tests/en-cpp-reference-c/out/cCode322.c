struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct _iobuf {
    void * _Placeholder;
};


static void _cake_memcpy(void * dest, const void * src, unsigned long n)
{
  char *csrc = (char *)src;
  char *cdest = (char *)dest;
  for (int i=0; i<n; i++) cdest[i] = csrc[i]; 
}

void *__cdecl memccpy(void * _Dst, void * _Src, int _Val, unsigned int _Size);


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
        for (; i != 5U; ++i)
        {
            void * to;

            to = memccpy(dest, src, terminal[i], 26U);
            printf("Terminal '%c' (%s):\t\"", terminal[i], to ? "found" : "absent");
            to = to ? to : dest + 26U;
            {
                char * from;
                from = dest;
                for (; from != to; ++from)
                putchar(isprint(*from) ? *from : 64);
            }
            puts("\"");
        }
    }
    puts("\n""Separate star names from distances (ly):");
    star_distance[0] = "Arcturus : 37";
    star_distance[1] = "Vega : 25";
    star_distance[2] = "Capella : 43";
    star_distance[3] = "Rigel : 860";
    star_distance[4] = "Procyon : 11";
    first = names_only;
    last = names_only + 64U;
    {
        unsigned int t;
        t = 0;
        for (; t != 5U; ++t)
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


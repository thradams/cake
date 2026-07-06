/* Cake x86_x64_gcc */

static void _cake_memcpy(void * dest, const void * src, unsigned long n)
{
  char *csrc;
  char *cdest;
  unsigned long i; 

  csrc = (char *)src;
  cdest = (char *)dest;
  for (i = 0; i < n; i++) cdest[i] = csrc[i]; 
}

extern void *memccpy(void * __dest, void * __src, int __c, unsigned int long __n);
extern int printf(char * __format, ...);
extern int putchar(int __c);
extern unsigned int short **__ctype_b_loc(void);
extern int puts(char * __s);

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
        unsigned int long i;

        i = 0;
        for (; i != 5L; ++i)
        {
            void * to;

            to = memccpy(dest, src, terminal[i], 26L);
            printf("Terminal '%c' (%s):\t\"", terminal[i], to ? "found" : "absent");
            to = to ? to : dest + 26L;
            {
                char * from;

                from = dest;
                for (; from != to; ++from)
                putchar(((*__ctype_b_loc())[(int)(*from)] & 16384) ? *from : 64);
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
    last = names_only + 64L;
    {
        unsigned int long t;

        t = 0;
        for (; t != 5L; ++t)
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



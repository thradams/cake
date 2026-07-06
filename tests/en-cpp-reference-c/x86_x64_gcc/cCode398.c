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

extern int *wcsncpy(int * __dest, int * __src, unsigned int long __n);
extern int puts(char * __s);
extern char *setlocale(int __category, char * __locale);
extern int printf(char * __format, ...);

int main(void)
{
    int src[3];
    int dest[6];
    long dest_size;

    _cake_memcpy(src, L"\xe3\x82\x8f\xe3\x82\x90", 3);
    dest[0] = 12354;
    dest[1] = 12356;
    dest[2] = 12358;
    dest[3] = 12360;
    dest[4] = 12362;
    dest[5] = 0;
    wcsncpy(dest, src, 4);
    puts("The contents of dest are: ");
    setlocale(6, "en_US.utf8");
    dest_size = 6L;
    {
        int * p;

        p = dest;
        for (; p - dest != 6L; ++p)
        {
            *p ? printf("%lc ", *p) : printf("\\0 ");
        }
    }
}



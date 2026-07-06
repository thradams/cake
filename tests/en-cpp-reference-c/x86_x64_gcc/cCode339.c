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

extern char *strncpy(char * __dest, char * __src, unsigned int long __n);
extern int printf(char * __format, ...);

int main(void)
{
    char src[3];
    char dest[6];
    char dest2[2];

    _cake_memcpy(src, "hi", 3);
    _cake_memcpy(dest, "abcdef", 6);
    strncpy(dest, src, 5);
    printf("strncpy(dest, src, 5) to a 6-byte dest gives : ");
    {
        unsigned int long n;

        n = 0;
        for (; n < 6L; ++n)
        {
            char c;

            c = dest[n];
            c ? printf("'%c' ", c) : printf("'\\0' ");
        }
    }
    printf("\nstrncpy(dest2, src, 2) to a 2-byte dst gives : ");
    strncpy(dest2, src, 2);
    {
        unsigned int long n;

        n = 0;
        for (; n < 2L; ++n)
        {
            char c;

            c = dest2[n];
            c ? printf("'%c' ", c) : printf("'\\0' ");
        }
    }
    printf("\n");
}



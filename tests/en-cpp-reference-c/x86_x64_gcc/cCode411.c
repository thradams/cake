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

extern int *wmemcpy(int * __s1, int * __s2, unsigned int long __n);
extern char *setlocale(int __category, char * __locale);
extern int printf(char * __format, ...);

int main(void)
{
    int from1[7];
    unsigned int long sz1;
    int from2[5];
    unsigned int long sz2;
    int to[];

    _cake_memcpy(from1, L"\xe0\xa4\xa8\xe0\xa4\xae\xe0\xa4\xb8\xe0\xa5\x8d\xe0\xa4\xa4\xe0\xa5\x87", 7);
    sz1 = 7L;
    _cake_memcpy(from2, L"\xd4\xb2\xd5\xa1\xd6\x80\xd6\x87", 5);
    sz2 = 5L;
    wmemcpy(to, from1, sz1);
    wmemcpy(to + sz1, from2, sz2);
    setlocale(6, "en_US.utf8");
    printf("Wide array contains: ");
    {
        unsigned int long n;

        n = 0;
        for (; n < 0 / 4L; ++n)
        if (to[n])
        {
            printf("%lc", to[n]);
        }
        else
        {
            printf("\\0");
        }
    }
    printf("\n");
}



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

extern char *setlocale(int __category, char * __locale);
extern int printf(char * __format, ...);
extern unsigned int long wcstombs(char * __s, int * __pwcs, unsigned int long __n);

int main(void)
{
    int src[5];
    char dst[11];
    int rtn_val;

    _cake_memcpy(src, L"z\u00df\u6c34\U0001f34c", 5);
    setlocale(6, "en_US.utf8");
    printf("wide-character string: '%ls'\n", src);
    {
        unsigned int long ndx;

        ndx = 0;
        for (; ndx < 5L; ++ndx)
        printf("   src[%2zu] = %#8x\n", ndx, src[ndx]);
    }
    rtn_val = wcstombs(dst, src, 11L);
    printf("rtn_val = %d\n", rtn_val);
    if (rtn_val > 0)
    {
        printf("multibyte string:  '%s'\n", dst);
    }
    {
        unsigned int long ndx;

        ndx = 0;
        for (; ndx < 11L; ++ndx)
        printf("   dst[%2zu] = %#2x\n", ndx, (unsigned char)dst[ndx]);
    }
}



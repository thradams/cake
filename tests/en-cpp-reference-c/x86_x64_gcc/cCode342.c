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

extern char *strrchr(char * __s, int __c);
extern int printf(char * __format, ...);

int main(void)
{
    char szSomeFileName[19];
    char * pLastSlash;
    char * pszBaseName;

    _cake_memcpy(szSomeFileName, "foo/bar/foobar.txt", 19);
    pLastSlash = strrchr(szSomeFileName, 47);
    pszBaseName = pLastSlash ? pLastSlash + 1 : szSomeFileName;
    printf("Base Name: %s", pszBaseName);
}



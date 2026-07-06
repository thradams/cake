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

extern unsigned int long wcsspn(int * __wcs, int * __accept);
extern char *setlocale(int __category, char * __locale);
extern int wprintf(int * __format, ...);

int main(void)
{
    int dest[9];
    int src[8];
    unsigned int long len;

    _cake_memcpy(dest, L"\xe7\x99\xbd\xe7\x8c\xab \xe9\xbb\x91\xe7\x8b\x97 \xe7\x94\xb2\xe8\x99\xab", 9);
    _cake_memcpy(src, L" \xe7\x8b\x97\xe7\x8c\xab \xe7\x99\xbd\xe9\xbb\x91 ", 8);
    len = wcsspn(dest, src);
    dest[len] = 0;
    setlocale(6, "en_US.utf8");
    wprintf(L"The length of maximum initial segment is %td.\nThe segment is \"%ls\".\n", len, dest);
}



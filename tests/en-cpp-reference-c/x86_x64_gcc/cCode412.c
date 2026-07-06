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
extern int *wmemmove(int * __s1, int * __s2, unsigned int long __n);

int main(void)
{
    int str[25];

    setlocale(6, "en_US.utf8");
    _cake_memcpy(str, L"\xce\xb1\xce\xb2\xce\xb3\xce\xb4\xce\xb5\xce\xb6\xce\xb7\xce\xb8\xce\xb9\xce\xba\xce\xbb\xce\xbc\xce\xbd\xce\xbe\xce\xbf\xcf\x80\xcf\x81\xcf\x83\xcf\x84\xcf\x85\xcf\x86\xcf\x87\xcf\x88\xcf\x89", 25);
    printf("%ls\n", str);
    wmemmove(str + 4, str + 3, 3);
    printf("%ls\n", str);
}



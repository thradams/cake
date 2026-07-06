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

extern int *wmemchr(int * __s, int __c, unsigned int long __n);
extern char *setlocale(int __category, char * __locale);
extern int printf(char * __format, ...);

int main(void)
{
    int str[23];
    unsigned int long sz;
    int target;
    int * result;

    _cake_memcpy(str, L"\xe8\xaf\xba\xe4\xb8\x8d\xe8\xbd\xbb\xe4\xbf\xa1\xef\xbc\x8c\xe6\x95\x85\xe4\xba\xba\xe4\xb8\x8d\xe8\xb4\x9f\xe6\x88\x91\0\xe8\xaf\xba\xe4\xb8\x8d\xe8\xbd\xbb\xe8\xae\xb8\xef\xbc\x8c\xe6\x95\x85\xe6\x88\x91\xe4\xb8\x8d\xe8\xb4\x9f\xe4\xba\xba\xe3\x80\x82", 23);
    sz = 23L;
    target = 35768;
    result = wmemchr(str, target, sz);
    if (result)
    {
        setlocale(6, "en_US.utf8");
        printf("Found '%lc' at position %td\n", target, result - str);
    }
}



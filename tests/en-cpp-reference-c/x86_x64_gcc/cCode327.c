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

extern int puts(char * __s);
extern void *memset(void * __s, int __c, unsigned int long __n);

int main(void)
{
    char str[23];

    _cake_memcpy(str, "ghghghghghghghghghghgh", 23);
    puts(str);
    memset(str, 97, 5);
    puts(str);
}



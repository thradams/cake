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
extern void *memmove(void * __dest, void * __src, unsigned int long __n);
extern void *malloc(unsigned int long __size);
extern int printf(char * __format, ...);

int main(void)
{
    char str[11];
    int * p;
    int arr[3];
    double d;
    signed int long n;

    _cake_memcpy(str, "1234567890", 11);
    puts(str);
    memmove(str + 4, str + 3, 3);
    puts(str);
    p = malloc(12L);
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    memmove(p, arr, 12L);
    d = 0.10000000000000001;
    memmove(&n, &d, 8L);
    printf("%a is %lx as an int64_t\n", d, n);
}



/* Cake x86_x64_gcc */
struct __tag3 {
    int k;
    int l;
    int a[2];
};

struct __tag4 {
    int i;
    struct __tag3  t;
};


static void _cake_memcpy(void * dest, const void * src, unsigned long n)
{
  char *csrc;
  char *cdest;
  unsigned long i; 

  csrc = (char *)src;
  cdest = (char *)dest;
  for (i = 0; i < n; i++) cdest[i] = csrc[i]; 
}

struct __tag3  x = {42,43,18,19};
extern int printf(char * __format, ...);

int main(void)
{
    struct __tag4  l;

    l.i = 1;
    _cake_memcpy(&l.t, &x, 16);
    l.t.l = 41;
    l.t.a[0] = 0;
    l.t.a[1] = 17;
    printf("l.t.k is %d\n", l.t.k);
}



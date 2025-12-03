/* Cake x86_msvc */
struct Y {
    double d;
};

struct X {
    struct Y  y;
    int i;
};


static void _cake_memcpy(void * dest, const void * src, unsigned int n)
{
  char *csrc;
  char *cdest;
  unsigned int i; 

  csrc = (char *)src;
  cdest = (char *)dest;
  for (i = 0; i < n; i++) cdest[i] = csrc[i]; 
}

void T2()
{
    struct Y  y;
    struct X  x;

    y.d = 3.0;
    _cake_memcpy(&x.y, &y, 8);
    x.i = 2;
}



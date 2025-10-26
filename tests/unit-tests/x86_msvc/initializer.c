/* Cake 0.12.26 x86_msvc */
struct A {
    int x;
    int y;
};

struct B {
    struct A  a;
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

int main(void)
{
    struct A  ia;
    struct B  b;

    ia.x = 1;
    ia.y = 2;
    _cake_memcpy(&b.a, &ia, 8);
    b.a.y = 42;
}



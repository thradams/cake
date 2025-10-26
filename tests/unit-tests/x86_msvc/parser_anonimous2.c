/* Cake 0.12.26 x86_msvc */
struct Y {
    int g;
};

union __tag1 {
    unsigned char  d;
    short e;
};

union __tag0 {
    char b;
    long c;
    union __tag1  __m0;
    double f;
};

struct X {
    int a;
    union __tag0  __m0;
    struct Y  y;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void f()
{
    struct X  s;

    _cake_zmem(&s, 24);
    s.__m0.f = 1;
}



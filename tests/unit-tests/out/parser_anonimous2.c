struct Y {
    int g;
};

union _struct_tag_1 {
    unsigned char   d;
    short e;
};

union _struct_tag_0 {
    char b;
    long c;
    union _struct_tag_1  __m0;
    double f;
};

struct X {
    int a;
    union _struct_tag_0  __m0;
    struct Y  y;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void f()
{
    struct X  s;

    _cake_zmem(&s, 24);
    s.__m0.f = 1;
}


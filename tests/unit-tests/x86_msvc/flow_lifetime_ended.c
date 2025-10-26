/* Cake 0.12.26 x86_msvc */
struct X {
    int i;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void f2()
{
    struct X * p;

    p = 0;
    {
        struct X  x;

        _cake_zmem(&x, 4);
        p = &x;
    }
    if (p->i)
    {
    }
}



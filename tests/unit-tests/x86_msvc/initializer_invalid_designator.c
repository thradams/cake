/* Cake x86_msvc */
struct X {
    int a;
    int b;
    int c;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void T13()
{
    struct X  x;

    _cake_zmem(&x, 12);
}



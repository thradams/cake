struct X {
    int a;
    int b;
    int c;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void T13()
{
    struct X  x;

    _cake_zmem(&x, 12);
}


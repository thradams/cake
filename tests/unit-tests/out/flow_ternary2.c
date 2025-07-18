struct X {
    char * text;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void f(int condition)
{
    struct X  x1;
    struct X  x2;
    struct X * p;

    _cake_zmem(&x1, 4);
    _cake_zmem(&x2, 4);
    p = condition ? &x1 : &x2;
}


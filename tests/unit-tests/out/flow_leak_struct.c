struct X {
    char * text;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

char *strdup(char * s);

void f(int a)
{
    struct X  x;

    _cake_zmem(&x, 4);
    x.text = strdup("a");
}


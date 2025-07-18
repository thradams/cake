struct X {
    char * name;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

char *strdup(char * s);

struct X make()
{
    struct X  x;

    _cake_zmem(&x, 4);
    x.name = strdup("text");
    return x;
}


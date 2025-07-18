struct X {
    char * name;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

char *strdup(char * s);
void free(void * p);

struct X make()
{
    struct X  x;

    _cake_zmem(&x, 4);
    x.name = strdup("text");
    free(x.name);
    return x;
}


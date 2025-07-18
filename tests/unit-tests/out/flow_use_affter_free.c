struct X {
    char * name;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void free(void * ptr);

void x_destroy(struct X * p)
{
    free(p->name);
}

void x_print(struct X * p)
{
}

char *strdup(char * s);

int main()
{
    struct X  x;

    _cake_zmem(&x, 4);
    x.name = strdup("a");
    x_destroy(&x);
    x_print(&x);
}


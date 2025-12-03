/* Cake x86_msvc */
struct X {
    char * text;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void free(void * p);

void x_destroy(struct X * x)
{
    free(x->text);
}


int main()
{
    struct X  x;

    _cake_zmem(&x, 4);
    x_destroy(&x);
}



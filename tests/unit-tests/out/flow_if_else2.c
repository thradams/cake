struct X {
    char * name;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

int rand();
void x_destroy(struct X * p);

void f()
{
    {
        struct X  x;

        _cake_zmem(&x, 4);
        if (rand())
        {
            x = f();
        }
        else
        {
            x = f();
        }
        x_destroy(&x);
    }
}


struct X {
    int i;
    void * p;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void f(struct X * p)
{
}

int main()
{
    struct X  x;

    _cake_zmem(&x, 8);
    f(&x);
}


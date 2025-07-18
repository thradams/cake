struct X {
    int i;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

struct X f()
{
    struct X  x;

    _cake_zmem(&x, 4);
    return x;
}

int main()
{
    f().i = 1;
}


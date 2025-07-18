struct X {
    char * name;
};

struct Y {
    int i;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

int main()
{
    struct Y  y1;
    struct Y  y2;
    struct X  x1;
    struct X  x2;

    _cake_zmem(&y1, 4);
    _cake_zmem(&y2, 4);
    y1 = y2;
    _cake_zmem(&x1, 4);
    _cake_zmem(&x2, 4);
    x1 = x2;
}


struct Y {
    char * p0;
    int * p2;
    double i2;
};

struct X {
    char * text;
    int * p1;
    int i;
    struct Y * pY;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

int main()
{
    struct X  x;

    _cake_zmem(&x, 16);
}


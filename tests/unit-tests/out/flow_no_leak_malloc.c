struct X {
    int i;
};

struct Y {
    struct X * p;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void *calloc(int i, int sz);
void free(void * p);

int main()
{
    struct Y  y;
    struct X * p;

    _cake_zmem(&y, 4);
    p = calloc(1, 4U);
    if (p)
    {
        y.p = p;
    }
    free(y.p);
}


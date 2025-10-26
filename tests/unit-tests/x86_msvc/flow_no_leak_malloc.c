/* Cake 0.12.26 x86_msvc */
struct X;

struct Y {
    struct X * p;
};

struct X {
    int i;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void *calloc(int i, int sz);
void free(void * p);

int main()
{
    struct Y  y;
    struct X * p;

    _cake_zmem(&y, 4);
    p = calloc(1, 4);
    if (p)
    {
        y.p = p;
    }
    free(y.p);
}



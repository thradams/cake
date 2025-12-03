/* Cake x86_msvc */
struct X {
    int i;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

struct X f()
{
    struct X  x;

    _cake_zmem(&x, 4);
    return x;
}


int i = 0;
int * p = 0;
int main()
{
    int a;
    struct X  x;
    struct X * px;

    _cake_zmem(&x, 4);
    px = 0;
}



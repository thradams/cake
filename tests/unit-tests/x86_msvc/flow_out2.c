/* Cake x86_msvc */
struct Y;

struct X {
    char * text;
    int * p1;
    int i;
    struct Y * pY;
};

struct Y {
    char * p0;
    int * p2;
    double i2;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void f(struct X * p);
void destroy(struct X * p);

int main()
{
    struct X  x;

    _cake_zmem(&x, 16);
    f(&x);
    destroy(&x);
}



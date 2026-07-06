/* Cake x86_msvc */
struct X {
    char * name;
};

struct Y {
    struct X  x;
    struct X * px;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void free(void * ptr);

void x_destroy(struct X * p)
{
    if (p)
    {
        free(p->name);
    }
}


void f(struct Y * p)
{
    x_destroy(p->px);
}


int main()
{
    struct Y  y;
    struct X * p;

    _cake_zmem(&y, 8);
    p = &y.x;
    x_destroy(&y.x);
}



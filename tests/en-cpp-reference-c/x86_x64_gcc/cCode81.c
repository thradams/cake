/* Cake x86_x64_gcc */

static void _cake_zmem(void *dest, unsigned long len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

extern int printf(char * __format, ...);

void f(int a[], int sz)
{
    {
        int i;

        i = 0;
        for (; i < sz; ++i)
        printf("%d\n", a[i]);
    }
}


void g(int (* a)[10])
{
    {
        int i;

        i = 0;
        for (; i < 10; ++i)
        printf("%d\n", (*a)[i]);
    }
}


int main(void)
{
    int a[10];

    _cake_zmem(&a, 40);
    f(a, 10);
    g(&a);
}



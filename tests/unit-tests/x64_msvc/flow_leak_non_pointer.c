/* Cake x64_msvc */

struct X {
    int i;
};

static void memset(void *dest, int ch, unsigned long long count);

void f()
{
    struct X x;

    memset(&x, 0, 4);
}

static void memset(void *dest, int ch, unsigned long long count)
{
    unsigned char *ptr;

    ptr = (unsigned char*)dest;
    while (count-- > 0)
    {
       *ptr++ = 0;
    }
}


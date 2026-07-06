/* Cake x64_msvc */

struct X {
    int a;
    int b;
    int c;
};

static void memset(void *dest, int ch, unsigned long long count);

void T13()
{
    struct X x;

    memset(&x, 0, 12);
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


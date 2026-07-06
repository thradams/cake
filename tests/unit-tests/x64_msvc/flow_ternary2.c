/* Cake x64_msvc */

struct X {
    char * text;
};

static void memset(void *dest, int ch, unsigned long long count);

void f(int condition)
{
    struct X x1;
    struct X x2;
    struct X * p;

    memset(&x1, 0, 8);
    memset(&x2, 0, 8);
    p = condition ? &x1 : &x2;
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


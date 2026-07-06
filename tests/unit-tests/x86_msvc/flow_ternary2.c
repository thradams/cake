/* Cake 0.14.05 x86_msvc */

struct X {
    char * text;
};

static void * memset(void *dest, int ch, unsigned int count);

void f(int condition)
{
    struct X x1;
    struct X x2;
    struct X * p;

    memset(&x1, 0, 4);
    memset(&x2, 0, 4);
    p = condition ? &x1 : &x2;
}

static void * memset(void *ptr, int value, unsigned int count)
{
    unsigned char *p;
    unsigned char v;

    p = (unsigned char *) ptr;
    v = (unsigned char) value;
    while (count--)
    {
        *p++ = v;
    }

    return ptr;
}

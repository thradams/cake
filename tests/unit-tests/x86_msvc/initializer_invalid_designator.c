/* Cake 0.13.29 x86_msvc */

struct X {
    int a;
    int b;
    int c;
};

static void * memset(void *dest, int ch, unsigned int count);

void T13()
{
    struct X x;

    memset(&x, 0, 12);
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

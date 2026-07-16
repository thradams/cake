/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    int i;
};

static void * memset(void *dest, int ch, unsigned int count);

void f2()
{
    struct X * p;

    p = 0;
    {
        struct X x;

        memset(&x, 0, 4);
        p = &x;
    }
    if (p->i)
    {
    }
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

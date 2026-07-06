/* Cake 0.13.29 x86_msvc */

struct X {
    int i;
};

static void * memset(void *dest, int ch, unsigned int count);

struct X f()
{
    struct X x;

    memset(&x, 0, 4);
    return x;
}

int main()
{
    f().i = 1;
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

/* Cake 0.14.05 x86_msvc */

struct X {
    int i;
    void * p;
};

static void * memset(void *dest, int ch, unsigned int count);

int main()
{
    struct X x;
    struct X __a;

    memset(&__a, 0, 8);
    x = __a;
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

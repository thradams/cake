/* Cake 0.14.05 x86_msvc */

struct X {
    void * text;
};

static void * memset(void *dest, int ch, unsigned int count);

void x_change(struct X * list);

int main()
{
    struct X x;

    memset(&x, 0, 4);
    x_change(&x);
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

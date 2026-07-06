/* Cake 0.14.05 x86_msvc */

struct X {
    char * name;
};

struct Y {
    int i;
};

static void * memset(void *dest, int ch, unsigned int count);

int main()
{
    struct Y y1;
    struct Y y2;
    struct X x1;
    struct X x2;

    memset(&y1, 0, 4);
    memset(&y2, 0, 4);
    y1 = y2;
    memset(&x1, 0, 4);
    memset(&x2, 0, 4);
    x1 = x2;
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

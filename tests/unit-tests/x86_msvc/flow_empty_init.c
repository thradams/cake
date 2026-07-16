/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct Y;

struct X {
    char * text;
    int * p1;
    int i;
    struct Y * pY;
};

struct Y {
    char * p0;
    int * p2;
    double i2;
};

static void * memset(void *dest, int ch, unsigned int count);

int main()
{
    struct X x;

    memset(&x, 0, 16);
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

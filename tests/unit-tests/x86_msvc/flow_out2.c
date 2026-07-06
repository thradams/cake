/* Cake 0.14.05 x86_msvc */

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

void f(struct X * p);
void destroy(struct X * p);

int main()
{
    struct X x;

    memset(&x, 0, 16);
    f(&x);
    destroy(&x);
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

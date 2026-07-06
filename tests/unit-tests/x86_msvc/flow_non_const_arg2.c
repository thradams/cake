/* Cake 0.14.05 x86_msvc */

struct X {
    int i;
    void * p;
};

static void * memset(void *dest, int ch, unsigned int count);

void f(struct X * p);
void free(void * p);

int main()
{
    struct X x;

    memset(&x, 0, 8);
    f(&x);
    free(x.p);
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

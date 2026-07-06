/* Cake 0.14.05 x86_msvc */

struct X {
    char * text;
};

static void * memset(void *dest, int ch, unsigned int count);

char *strdup(char * s);

void f(int a)
{
    struct X x;

    memset(&x, 0, 4);
    x.text = strdup("a");
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

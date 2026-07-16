/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * name;
};

static void * memset(void *dest, int ch, unsigned int count);

char *strdup(char * s);
void free(void * p);

struct X make()
{
    struct X x;

    memset(&x, 0, 4);
    x.name = strdup("text");
    free(x.name);
    return x;
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

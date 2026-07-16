/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

struct X {
    char * name;
};

static void * memset(void *dest, int ch, unsigned int count);

void free(void * ptr);

void x_destroy(struct X * p)
{
    free(p->name);
}

void x_print(struct X * p)
{
}

char *strdup(char * s);

int main()
{
    struct X x;

    memset(&x, 0, 4);
    x.name = strdup("a");
    x_destroy(&x);
    x_print(&x);
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
